/*
 * Copyright (C) 2007 Rolf Eike Beer <kde@opensource.sf-tec.de>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef GPGPROC_H
#define GPGPROC_H

#include <QString>
#include <QStringList>

#include "klinebufferedprocess.h"

/**
 * @brief A interface to GnuPG handling UTF8 recoding correctly
 *
 * This class handles the GnuPG formatted UTF8 output correctly.
 * GnuPG recodes some characters as \\xnn where nn is the hex representation
 * of the character. This can't be fixed up simply when using QString as
 * QString already did it's own UTF8 conversion. Therefore we replace this
 * sequences by their corresponding character so QString will work just fine.
 *
 * As we know that GnuPG limits it's columns by QLatin1Char( ':' ) we skip \\x3a. Since this
 * is an ascii character (single byte) the replacement can be done later without
 * problems after the line has been split into pieces.
 *
 * @author Rolf Eike Beer
 */
class GPGProc : public KLineBufferedProcess
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent parent object
     * @param binary path to GnuPG binary or QString() to use the configured
     */
    explicit GPGProc(QObject *parent = 0, const QString &binary = QString());

    /**
     * Destructor
     */
    ~GPGProc();

    /**
     *  Starts the process
     */
    void start();

    /**
     * Reads a line of text (excluding '\\n').
     *
     * Use readln() in response to a readReady() signal.
     * You may use it multiple times if more than one line of data is
     * available.
     *
     * readln() never blocks.
     *
     * @param line is used to store the line that was read.
     * @param colons recode also colons
     * @return the number of characters read, or -1 if no data is available.
     */
    int readln(QString &line, const bool colons = false);

    /**
     * Reads a line of text and splits it into parts.
     *
     * Use readln() in response to a readReady() signal.
     * You may use it multiple times if more than one line of data is
     * available.
     *
     * readln() never blocks.
     *
     * @param l is used to store the parts of the line that was read.
     * @return the number of characters read, or -1 if no data is available.
     */
    int readln(QStringList &l);

    /**
     * Recode a line from GnuPG encoding to UTF8
     *
     * @param a data to recode
     * @param colons recode also colons
     * @return recoded string
     */
    static QString recode(QByteArray a, const bool colons = true);

    /**
     * Reset the class to the state it had right after creation
     * @param binary path to GnuPG binary or empty string to use the configured one
     */
    void resetProcess(const QString &binary = QString());

    /**
     * @brief parse GnuPG version string and return version as number
     * @param vstr version string
     * @return -1 if vstr is empty, -2 on parse error, parsed number on success
     *
     * The version string must be in format A.B.C with A, B, and C numbers. The
     * returned number is A * 65536 + B * 256 + C.
     */
    static int gpgVersion(const QString &vstr);
    /**
     * @brief get the GnuPG version string of the given binary
     * @param binary name or path to GnuPG binary
     * @return version string or empty string on error
     *
     * This starts a GnuPG process and asks the binary for version information.
     * The returned string is the version information without any leading text.
     */
    static QString gpgVersionString(const QString &binary);
    /**
     * @brief find users GnuPG directory
     * @param binary name or path to GnuPG binary
     * @return path to directory
     *
     * Use this function to find out where GnuPG would store it's configuration
     * and data files. The returned path always ends with a '/'.
     */
    static QString getGpgHome(const QString &binary);

    /**
     * @brief run GnuPG and check if it complains about anything
     * @param binary the GnuPG binary to run
     * @return the error message GnuPG gave out (if any)
     */
    static QString getGpgStartupError(const QString &binary);

signals:
    /**
     * Emitted when the process is ready for reading.
     * The signal is only emitted if at least one complete line of data is ready.
     * @param p the process that emitted the signal
     */
    void readReady();

    /**
     * Emitted when the process has finished
     * @param p the process that emitted the signal
     */
    void processExited();

protected slots:
    void finished();
    void received();
};

#endif // GPGPROC_H
