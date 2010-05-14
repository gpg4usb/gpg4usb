#include "attachmenttablemodel.h"

/** compare with http://doc.qt.nokia.com/4.6/itemviews-addressbook.html
 */

AttachmentTableModel::AttachmentTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

AttachmentTableModel::AttachmentTableModel(QList<MimePart> mimeparts, QObject *parent) :
    QAbstractTableModel(parent)
{
    listOfMimeparts = mimeparts;
}

AttachmentTableModel::AttachmentTableModel(QString iconpath, QObject *parent) :
    QAbstractTableModel(parent)
{
    iconPath = iconpath;
}


void AttachmentTableModel::add(MimePart mp) {
   listOfMimeparts.append(mp);
   //QModelIndex changedIndex0 = createIndex(listOfMimeparts.size(), 0);
   //QModelIndex changedIndex1 = createIndex(listOfMimeparts.size(), 1);

   //emit(dataChanged(changedIndex0, changedIndex1));
   // TODO: check the data-changed signal
   reset();
}

MimePart AttachmentTableModel::getSelectedMimePart(QModelIndex index){
    return listOfMimeparts.at(index.row());
}

MimePart AttachmentTableModel::getMimePart(int index){
    return listOfMimeparts.at(index);
}

/*QList<MimePart> AttachmentTableModel::getSelectedMimeParts(QModelIndexList indexes){

    foreach(QModelIndex index, indexes) {
        qDebug() << "ir: "<< index.row();
    }

}*/

int AttachmentTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return listOfMimeparts.size();
}

int AttachmentTableModel::columnCount(const QModelIndex &parent) const
 {
     Q_UNUSED(parent);
     return 2;
 }

QVariant AttachmentTableModel::data(const QModelIndex &index, int role) const
{

    //qDebug() << "called, index: " << index.column();

    if (!index.isValid())
        return QVariant();

    if (index.row() >= listOfMimeparts.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        MimePart mp = listOfMimeparts.at(index.row());

        if (index.column() == 0)
            return mp.getParam("Content-Type", "name");
        if (index.column() == 1)
            return mp.getValue("Content-Type");

    }

    // set icon
    // TODO more generic matching, e.g. for audio
    if (role == Qt::DecorationRole && index.column() == 0) {
        MimePart mp = listOfMimeparts.at(index.row());
        QString icon;
        if(mp.getValue("Content-Type").startsWith("image")) {
           icon = iconPath + "/mimetypes/image-x-generic.png";
       } else {
           icon = mp.getValue("Content-Type").replace("/", "-");
           icon = iconPath + "/mimetypes/" + icon + ".png";
       }
       if(!QFile::exists(icon)) icon = iconPath + "/mimetypes/unknown.png";
       return QIcon(icon);
    }

    return QVariant();
}

QVariant AttachmentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    //qDebug() << "called, section: " << section;
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Filename");

            case 1:
                return tr("Contenttype");

            default:
                return QVariant();
        }
    }
    return QVariant();
}
