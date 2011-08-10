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


#ifndef BOOKMARKWIDGET_H
#define BOOKMARKWIDGET_H

// Qt Includes
#include <QtGui/QMenu>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QComboBox>

//Nepomuk Includes
#include <Nepomuk/Resource>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Tag>
#include <Nepomuk/File>

//KDE Includes
#include <KUrlRequester>

// Forward Declarations
class KBookmark;
class KLineEdit;


class BookmarkWidget : public QMenu
{
    Q_OBJECT

public:
    explicit BookmarkWidget(const KBookmark &bookmark, QWidget *parent = 0);
    virtual ~BookmarkWidget();

    void showAt(const QPoint &pos);
    void updateTagList();

Q_SIGNALS:
    void updateIcon();

private Q_SLOTS:
    void accept();
    void removeBookmark();
    void addTagSlot( QString );
    void setRatingSlot( int );
    void addCommentSlot();
    void linkToResourceSlot();

private:
    KBookmark *m_bookmark;
    KLineEdit *m_name;
    KLineEdit *m_tagLine;
    QPlainTextEdit *m_commentEdit;
    KUrlRequester *m_resourceSelect;
    QLabel *m_tagListLabel;
    Nepomuk::Resource m_nfoResource;


};

#endif // BOOKMARKWIDGET_H
