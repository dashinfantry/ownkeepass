/***************************************************************************
**
** Copyright (C) 2012 Marko Koschak (marko.koschak@tisno.de)
** All rights reserved.
**
** This file is part of ownKeepass.
**
** ownKeepass is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** ownKeepass is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with ownKeepass.  If not, see <http://www.gnu.org/licenses/>.
**
***************************************************************************/

#ifndef KDBENTRY_H
#define KDBENTRY_H

#include <QAbstractItemModel>
#include "private/AbstractDatabaseInterface.h"
#include "ownKeepassGlobal.h"

using namespace ownKeepassPublic; // for using baseRole

namespace kpxPublic {

class AdditionalAttributeItem
{
public:
    AdditionalAttributeItem(QString key, QString value, bool editKeyMode = false)
        : m_key(key),
          m_value(value),
          m_edit_key_mode(editKeyMode),
          m_original_key(key),
          m_original_value(value),
          m_to_be_deleted(false)
    {
        static int itemCount = 0;
        m_index = itemCount;
        itemCount++;
    }
    virtual ~AdditionalAttributeItem() {}

    QVariant get(const int role) const;
    bool set(const QVariant & value, const int role);
    static QHash<int, QByteArray> createRoles();

    QString m_key;
    QString m_value;
    bool m_edit_key_mode;
    int m_index;

    QString m_original_key;
    QString m_original_value;
    bool m_to_be_deleted;
    bool m_modified;
};

class KdbEntry : public QAbstractListModel
{
    Q_OBJECT

public:
    Q_PROPERTY(QString entryId READ getEntryId WRITE setEntryId STORED true SCRIPTABLE true)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle STORED true SCRIPTABLE true NOTIFY entryDataLoaded)
    Q_PROPERTY(QString url READ getUrl WRITE setUrl STORED true SCRIPTABLE true NOTIFY entryDataLoaded)
    Q_PROPERTY(QString userName READ getUserName WRITE setUserName STORED true SCRIPTABLE true NOTIFY entryDataLoaded)
    Q_PROPERTY(QString password READ getPassword WRITE setPassword STORED true SCRIPTABLE true NOTIFY entryDataLoaded)
    Q_PROPERTY(QString notes READ getNotes WRITE setNotes STORED true SCRIPTABLE true NOTIFY entryDataLoaded)
    Q_PROPERTY(QString iconUuid READ getIconUuid WRITE setIconUuid STORED true SCRIPTABLE true NOTIFY entryDataLoaded)
    Q_PROPERTY(QString groupId READ getGroupId WRITE setGroupId NOTIFY entryDataLoaded)
    Q_PROPERTY(bool edited READ getEdited NOTIFY dataEdited)

    // for list model
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY isEmptyChanged)

public:
    Q_INVOKABLE void loadEntryData();
    Q_INVOKABLE void saveEntryData();
    Q_INVOKABLE void createNewEntry();
    Q_INVOKABLE void deleteEntry();
    Q_INVOKABLE void moveEntry(QString newGroupId);
    Q_INVOKABLE void clearData();

    // for list model
    Q_INVOKABLE void clearListModel();

signals:
    // signals to QML
    void dataEdited();
    void entryDataLoaded(int result,
                         QString errorMsg);
    void entryDataSaved(int result,
                        QString errorMsg);
    void newEntryCreated(int result,
                         QString errorMsg,
                         QString newEntryId);
    void entryDeleted(int result,
                      QString errorMsg);
    void entryMoved(int result,
                    QString errorMsg);

    // signals to interface of database client
    void loadEntryFromKdbDatabase(QString entryId);
    void saveEntryToKdbDatabase(QString entryId,
                                QStringList keys,
                                QStringList values,
                                QStringList keysToDelete,
                                QString iconUuid);
    void createNewEntryInKdbDatabase(QStringList keys,
                                     QStringList values,
                                     QString parentgroupId,
                                     QString iconUuid);
    void deleteEntryFromKdbDatabase(QString entryId);
    void moveEntryInKdbDatabase(QString entryId, QString newGroupId);

    // for list model
    void modelDataChanged();
    void isEmptyChanged();

public slots:
    // signals from interface of database client
    void slot_entryDataLoaded(int result,
                              QString errorMsg,
                              QString entryId,
                              QStringList keys,
                              QStringList values,
                              QString iconUuid);
    void slot_entryDataSaved(int result,
                             QString errorMsg,
                             QString entryId);
    void slot_entryDeleted(int result,
                           QString errorMsg,
                           QString entryId);
    void slot_entryMoved(int result,
                         QString errorMsg,
                         QString entryId);
    void slot_newEntryCreated(int result,
                              QString errorMsg,
                              QString entryId);
    void slot_disconnectFromDatabaseClient();

public:
    KdbEntry(QObject *parent = 0);
    virtual ~KdbEntry();

    QString getEntryId() const { return m_entryId; }
    void setEntryId(const QString value) { m_entryId = value; }
    QString getTitle() const { return m_title; }
    void setTitle(const QString value);
    QString getUrl() const { return m_url; }
    void setUrl(const QString value);
    QString getUserName() const { return m_userName; }
    void setUserName(const QString value);
    QString getPassword() const { return m_password; }
    void setPassword(const QString value);
    QString getNotes() const { return m_notes; }
    void setNotes(const QString value);
    QString getIconUuid() const { return m_iconUuid; }
    void setIconUuid(const QString value);
    QString getGroupId() const { return m_groupId; }
    void setGroupId(const QString value);
    bool getEdited() { checkIfEdited(); return m_edited; }

    // for reading list model
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool isEmpty();
    // Overwrite function to set role names
    virtual QHash<int, QByteArray> roleNames() const { return AdditionalAttributeItem::createRoles(); }

    // for editable list model
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
private:
    bool connectToDatabaseClient();
    void disconnectFromDatabaseClient();
    void checkIfEdited();
    bool checkIfAdditionalAttibuteItemsModified();

private:
    QList<AdditionalAttributeItem> m_additional_attribute_items;
    QString m_entryId;
    QString m_groupId;

    QString m_title;
    QString m_url;
    QString m_userName;
    QString m_password;
    QString m_notes;
    QString m_iconUuid;
    QString m_original_title;
    QString m_original_url;
    QString m_original_userName;
    QString m_original_password;
    QString m_original_notes;
    QString m_original_iconUuid;
    bool m_title_modified;
    bool m_url_modified;
    bool m_userName_modified;
    bool m_password_modified;
    bool m_notes_modified;
    bool m_iconUuid_modified;

    bool m_connected;
    bool m_new_entry_triggered;
    bool m_edited;
};

// inline implementations
inline QVariant AdditionalAttributeItem::get(const int role) const
{
    switch (role) {
    case baseRole:
        return m_key;
    case baseRole + 1:
        return m_value;
    case baseRole + 2:
        return m_edit_key_mode;
    case baseRole + 3:
        return m_index;
    case baseRole + 4:
        return m_to_be_deleted;
    case baseRole + 5:
        return m_modified;
    }
    return QVariant();
}

inline bool AdditionalAttributeItem::set(const QVariant & value, const int role)
{
    switch (role) {
    case baseRole:
        m_key = value.toString();
        if (m_key != m_original_key || m_value != m_original_value) {
            m_modified = true;
        } else {
            m_modified = false;
        }
        return true;
    case baseRole + 1:
        m_value = value.toString();
        if (m_key != m_original_key || m_value != m_original_value) {
            m_modified = true;
        } else {
            m_modified = false;
        }
        return true;
    case baseRole + 2:
        m_edit_key_mode = value.toBool();
        return true;
    case baseRole + 3:
        // m_index is not editable
        return false;
    case baseRole + 4:
        m_to_be_deleted = value.toBool();
        return true;
    case baseRole + 5:
        // m_modified is not editable
        return false;
    }
    return false;
}

inline QHash<int, QByteArray> AdditionalAttributeItem::createRoles()
{
    QHash<int, QByteArray> roles;
    roles[baseRole]     = "key";
    roles[baseRole + 1] = "value";
    roles[baseRole + 2] = "editKeyMode";
    roles[baseRole + 3] = "index";
    roles[baseRole + 4] = "toBeDeleted";
    roles[baseRole + 5] = "modified";
    return roles;
}

}
#endif // KDBENTRY_H
