#ifndef KEYTABLEMODEL_H
#define KEYTABLEMODEL_H

#include "context.h"
#include <QAbstractTableModel>

class KeyTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    KeyTableModel(GpgME::Context *ctx, QString iconpath, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    GpgKeyList keys;
    QString iconPath;
    GpgME::Context *mCtx;

};

#endif // KEYTABLEMODEL_H
