/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2010-2011 by Yoann Laissus <yoann dot laissus at gmail dot com>
*
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
* accepted by the membership of KDE e.V. (or its successor approved
* by the membership of KDE e.V.), which shall act as a proxy
* defined in Section 14 of version 3 of the license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* ============================================================ */


// Auto Includes
#include "bookmarkwidget.h"
#include "bookmarkwidget.moc"

// Local includes
#include "application.h"
#include "bookmarkprovider.h"
#include "bookmarkowner.h"
#include "mainwindow.h"
#include "resourcelinkdialog.h"

// KDE Includes
#include <KLocalizedString>
#include <KIcon>
#include <KLineEdit>
#include <Nepomuk/KRatingWidget>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryServiceClient>
#include <KFileDialog>
#include <KCompletion>
#include <KConfigDialog>
#include <KConfigSkeleton>

#include <QCompleter>
#include <QStringList>


BookmarkWidget::BookmarkWidget( const KBookmark &bookmark, QWidget *parent )
        : QMenu( parent )
        , m_bookmark( new KBookmark( bookmark ) )
{
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedWidth( 400 );
    setTitle( "Fancy Bookmarking" );
    QGridLayout *layout = new QGridLayout( this );

    m_nfoResource = (QUrl)m_bookmark->url();

    // Title
    QVBoxLayout *vLayout = new QVBoxLayout( this );
    QLabel *bookmarkInfo = new QLabel( this );
    bookmarkInfo->setText( i18n( "<h4> Edit this Bookmark </h4>" ) );
    bookmarkInfo->setAlignment( Qt::AlignCenter );
    vLayout->addWidget( bookmarkInfo );
    layout->addLayout( vLayout, 1, 2 );

    // Remove button
    QPushButton *removeButton = new QPushButton( this );
    removeButton->setText( i18n( "Remove" ) );
    connect(removeButton, SIGNAL( clicked() ), this, SLOT( removeBookmark() ));
    layout->addWidget( removeButton, 1, 3);

    // Bookmark name
    QLabel *nameLabel = new QLabel( this );
    nameLabel->setText( i18n( "Name:" ) );
    nameLabel->setAlignment( Qt::AlignLeft );
    m_name = new KLineEdit( this );
    if ( m_bookmark->isNull() )
    {
        m_name->setEnabled( false );
    }
    else
    {
        m_name->setText( m_bookmark->text() );
        m_name->setFocus();
    }
    layout->addWidget( nameLabel, 4, 1 );
    layout->addWidget( m_name, 4, 2 );

    //Create tags
    QLabel *tagLabel = new QLabel( this );
    tagLabel->setText( i18n( "Tags:" ) );
    tagLabel->setAlignment( Qt::AlignLeft );
    m_tagLine = new KLineEdit( this );
    m_tagLine->setPlaceholderText( i18n( "add tags" ) );
    connect( m_tagLine,SIGNAL( returnPressed( QString ) ),this,SLOT( addTagSlot( QString ) ) );
    //    QPushButton *addTagButton = new QPushButton( this );
//    addTagButton->setText( i18n( "+" ) );
    QStringList tList;
    QList<Nepomuk::Tag> tagList = Nepomuk::Tag::allTags();
    Q_FOREACH(Nepomuk::Tag t,tagList) {
        tList.append( t.label() );
    }
    QCompleter *completeTag = new QCompleter( tList );
    completeTag->setCompletionMode(QCompleter::PopupCompletion);
    m_tagLine->setCompleter(completeTag);
    //TODO::Create PLUS icon instead of button
    //connect(addTagButton, SIGNAL( clicked() ), this, SLOT( addTagSlot() ) );
    layout->addWidget( tagLabel, 5, 1 );
    layout->addWidget( m_tagLine, 5, 2 );
 //   layout->addWidget( addTagButton, 5, 3 );
    m_tagListLabel = new QLabel( this );
    updateTagList();
    layout->addWidget( m_tagListLabel, 6, 2 );

    //Add comments
    QLabel *commentLabel = new QLabel( this );
    commentLabel->setText( i18n( "Description:" ) );
    commentLabel->setAlignment( Qt::AlignCenter );
    m_commentEdit = new QPlainTextEdit( this );
    if( !m_nfoResource.description().isEmpty() ) {
        m_commentEdit->setPlainText( m_nfoResource.description() );
    }
//    QPushButton *addCommentButton = new QPushButton( this );
//    addCommentButton->setText( i18n( "Add" ) );
//    connect( addCommentButton, SIGNAL( clicked() ), this, SLOT( addCommentSlot() ) );
    layout->addWidget( commentLabel, 7, 1 );
    layout->addWidget( m_commentEdit, 7, 2 );
    //layout->addWidget( addCommentButton, 7, 3 );

    //Rate this page
    KRatingWidget *ratingWidget = new KRatingWidget( this );
    if( m_nfoResource.rating() != NULL ) {
        ratingWidget->setRating( m_nfoResource.rating() );
    }
    connect( ratingWidget, SIGNAL( ratingChanged( int ) ), this, SLOT( setRatingSlot( int ) ) );
    ratingWidget->setToolTip( i18n( "Rate this page" ) );
    layout->addWidget( ratingWidget, 1, 1);

    //Link to resources
    QPushButton *linkToResourceButton = new QPushButton( this );
    linkToResourceButton->setText( i18n( "Link to Resource" ) );
    linkToResourceButton->setToolTip( i18n( "Open Resource Link Dialog" ) );
    connect( linkToResourceButton, SIGNAL( clicked() ), this, SLOT( linkToResourceSlot() ) );
    layout->addWidget( linkToResourceButton, 8, 2 );

    QLabel *msgLabel = new QLabel( this );
    layout->addWidget( msgLabel, 9, 2 );

    // Ok & Cancel buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    layout->addWidget(buttonBox, 10, 2);

    setLayout(layout);
}


BookmarkWidget::~BookmarkWidget()
{
    delete m_bookmark;
}


void BookmarkWidget::updateTagList()
{
    QList<Nepomuk::Tag> oldTags = m_nfoResource.tags();
    QStringList oldTagList;
    Q_FOREACH( Nepomuk::Tag oldTag, oldTags ) {
        oldTagList.append( oldTag.label() );
    }
    QString oldTagListJoin = oldTagList.join(", ");
    m_tagListLabel->setText( oldTagListJoin );
}


void BookmarkWidget::showAt(const QPoint &pos)
{
    adjustSize();

    QPoint p(pos.x() - width(), pos.y() + 10);
    move(p);
    show();
}


void BookmarkWidget::accept()
{
    if (!m_bookmark->isNull() && m_name->text() != m_bookmark->fullText())
    {
        m_bookmark->setFullText(m_name->text());
        rApp->bookmarkProvider()->bookmarkManager()->emitChanged();

    }
    if( !m_commentEdit->toPlainText().isEmpty() ) {
        m_nfoResource.setDescription(m_commentEdit->toPlainText());
    }
    if( !m_tagLine->text().isEmpty() ) {
        Nepomuk::Tag newTag( m_tagLine->text() );
        m_nfoResource.addTag( newTag );
        m_tagLine->clear();
        m_tagLine->setPlaceholderText( i18n( "Add another tag" ) );
    }

    close();
}


void BookmarkWidget::removeBookmark()
{
    rApp->bookmarkProvider()->bookmarkOwner()->deleteBookmark(*m_bookmark);
    close();
    emit updateIcon();
}


void BookmarkWidget::addTagSlot(QString tag)
{
    Nepomuk::Tag newTag( tag );
    m_nfoResource.addTag( newTag );
    m_tagLine->clear();
    m_tagLine->setPlaceholderText( i18n( "Add another tag" ) );
    updateTagList();
}


void BookmarkWidget::setRatingSlot( int rate )
{
    m_nfoResource.setRating(rate);
}


void BookmarkWidget::addCommentSlot()
{
    //m_nfoResource.setDescription(m_commentEdit->toPlainText());
}


void BookmarkWidget::linkToResourceSlot()
{
    Nepomuk::ResourceLinkDialog r( m_nfoResource );
    r.exec();
//    KConfigSkeleton *config =new KConfigSkeleton("test");
//    KConfigDialog *k = new KConfigDialog(this ,"test", config);
//    k->setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help );
//    k->exec();

}


