#ifndef ATTACHMENTTABLEMODEL_H
#define ATTACHMENTTABLEMODEL_H

#include "mime.h"

#include <QAbstractTableModel>


class AttachmentTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    AttachmentTableModel(QObject *parent = 0);
    AttachmentTableModel(QList<MimePart> mimeparts, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void add(MimePart mp);

private:
    QList<MimePart> listOfMimeparts;

};

#endif // ATTACHMENTTABLEMODEL_H
