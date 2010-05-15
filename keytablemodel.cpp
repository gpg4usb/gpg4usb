#include "keytablemodel.h"

KeyTableModel::KeyTableModel(GpgME::Context *ctx, QString iconpath, QObject *parent) :
                             QAbstractTableModel(parent)
{
    mCtx = ctx;
    iconPath = iconpath;
    keys = mCtx->listKeys();
}

int KeyTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return keys.size();
}

int KeyTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant KeyTableModel::data(const QModelIndex &index, int role) const
{

    //qDebug() << "called, index: " << index.column();

    if (!index.isValid())
        return QVariant();

    if (index.row() >= keys.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        GpgKey key = keys.at(index.row());

        if (index.column() == 0)
            return "";
        if (index.column() == 1)
            return "";
        if (index.column() == 2)
            return key.name;
        if (index.column() == 3)
            return key.email;


    }

    // set icon
    if (role == Qt::DecorationRole && index.column() == 1) {
        //return QIcon(icon);
        GpgKey key = keys.at(index.row());
        if(key.privkey) return QIcon(iconPath + "kgpg_key2.png");
    }

    return QVariant();
}

QVariant KeyTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {

        case 0:
            return tr("");

        case 1:
            return tr("");

        case 2:
            return tr("Name");

        case 3:
            return tr("EMail");

        default:
            return QVariant();
        }
    }
    return QVariant();
}
