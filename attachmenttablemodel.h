#ifndef ATTACHMENTTABLEMODEL_H
#define ATTACHMENTTABLEMODEL_H

#include "mime.h"

#include <QAbstractTableModel>
#include <QStandardItem>
#include <QIcon>
#include <QFile>


class AttachmentTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    AttachmentTableModel(QObject *parent = 0);
    AttachmentTableModel(QString iconpath, QObject *parent = 0);
    AttachmentTableModel(QList<MimePart> mimeparts, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void add(MimePart mp);
    MimePart getSelectedMimePart(QModelIndex index);
    MimePart getMimePart(int index);
    //QList<MimePart> getSelectedMimeParts(QModelIndexList indexes);

private:
    QList<MimePart> listOfMimeparts;
    QString iconPath;

};

#endif // ATTACHMENTTABLEMODEL_H
