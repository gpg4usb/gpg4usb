/*
 *      mime.cpp
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include "mime.h"
#include <QDebug>
#include <QHashIterator>
#include <QTextCodec>

Mime::Mime(QByteArray *message)
{
  splitParts(message);
    /*
  mMessage = message;
  int bStart = mMessage->indexOf("boundary=\"") + 10 ;
  int bEnd = mMessage->indexOf("\"\n", bStart );

  qDebug() << "bStart: " << bStart << " bEnd: " << bEnd;
  mBoundary = new QByteArray(mMessage->mid(bStart, bEnd - bStart));
  qDebug() << "boundary: " << *mBoundary;

  Part *p1 = new Part();

  int nb = mMessage->indexOf(*mBoundary, bEnd) + mBoundary->length() +1 ;
  qDebug() << "nb: " << nb;
  int eh = mMessage->indexOf("\n\n", nb);
  qDebug() << "eh: " << eh;
  QByteArray *header = new QByteArray(mMessage->mid(nb , eh - nb));
  qDebug() << "header:" << header;

  // split header at newlines
  foreach(QByteArray tmp , header->split(* "\n")) {
    // split lines at :
    QList<QByteArray> tmp2 = tmp.split(* ":");
    p1->header.insert(QString(tmp2[0].trimmed()), QString(tmp2[1].trimmed()));
  }

  QHashIterator<QString, QString> i(p1->header);
   while (i.hasNext()) {
       i.next();
       qDebug() << "found: " << i.key() << ":" << i.value() << endl;
  }

  int nb2 = mMessage->indexOf(*mBoundary, eh);

  p1->body = mMessage->mid(eh , nb2 - eh);

  QTextCodec *codec = QTextCodec::codecForName("ISO-8859-15");
  QString qs = codec->toUnicode(p1->body);
  qDebug() << "body: " << qs;
*/
}

Mime::~Mime()
{

}

void Mime::splitParts(QByteArray *message) {
    int pos1, pos2, headEnd;
    MimePart p_tmp;

    // find the boundary
    pos1 = message->indexOf("boundary=\"") + 10 ;
    pos2 = message->indexOf("\"\n", pos1 );
    QByteArray boundary = message->mid(pos1, pos2 - pos1);
    //qDebug() << "boundary: " << boundary;

    while ( pos2 > pos1 ) {

        pos1 = message->indexOf(boundary, pos2) + boundary.length() +1 ;
        headEnd = message->indexOf("\n\n", pos1);
        if(headEnd < 0)
            break;
        QByteArray header = message->mid(pos1 , headEnd - pos1);

        p_tmp.header = parseHeader(&header);

        pos2 = message->indexOf(boundary, headEnd);
        p_tmp.body = message->mid(headEnd , pos2 - headEnd);

        mPartList.append(p_tmp);
    }
}

QList<HeadElem> Mime::parseHeader(QByteArray *header){

    QList<HeadElem> ret;

    /** http://www.aspnetmime.com/help/welcome/overviewmimeii.html :
     * If a line starts with any white space, that line is said to be 'folded' and is actually
     * part of the header above it.
     */
    header->replace("\n ", " ");

    //split header at newlines
    foreach(QByteArray line , header->split(* "\n")) {
        HeadElem elem;
        //split lines at :
        QList<QByteArray> tmp2 = line.split(* ":");
            elem.name = tmp2[0].trimmed();
            if(tmp2[1].contains(';')) {
                // split lines at ;
                // TODO: what if ; is inside ""
                QList<QByteArray> tmp3 = tmp2[1].split(* ";");
                elem.value = QString(tmp3.takeFirst().trimmed());
                foreach(QByteArray tmp4, tmp3) {
                    QList<QByteArray> tmp5 = tmp4.split(* "=");
                    elem.params.insert(QString( tmp5[0].trimmed() ), QString(tmp5[1].trimmed()));
                }
            } else {
                elem.value = tmp2[1].trimmed();
            }
            ret.append(elem);
        }
    return ret;
}

bool Mime::isMultipart(QByteArray *message)
{
    return message->startsWith("Content-Type: multipart/mixed;");
}

