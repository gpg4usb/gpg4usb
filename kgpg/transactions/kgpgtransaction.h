/*
 * Copyright (C) 2008,2009 Rolf Eike Beer <kde@opensource.sf-tec.de>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KGPGTRANSACTION_H
#define KGPGTRANSACTION_H

#include <QObject>
#include <QString>
#include <QStringList>

class GPGProc;
class KGpgSignTransactionHelper;
class KGpgTransactionPrivate;
class QByteArray;
class QProcess;

/**
 * @brief Process one GnuPG operation
 *
 * This class encapsulates one GnuPG operation. It will care for all
 * interaction with the gpg process. Everything you have to care about
 * is to set up the object properly, call start() and catch the done signal.
 *
 * This is an abstract base class for specific operations that implements
 * the basic I/O loop, the process setup and interaction and some convenience
 * members to set extra arguments for the process.
 *
 * If you want to add a new operation create a child class that implements
 * nextLine(). Ususally you also need a constructor that takes some information
 * like the id of the key to modify.
 *
 * @author Rolf Eike Beer
 */
class KGpgTransaction: public QObject {
	Q_OBJECT

	friend class KGpgTransactionPrivate;
	friend class KGpgSignTransactionHelper;

	Q_DISABLE_COPY(KGpgTransaction)

public:
	/**
	 * @brief return codes common to many transactions
	 *
	 * Every transaction may define additional return codes, which
	 * should start at TS_COMMON_END + 1.
	 */
	enum ts_transaction {
		TS_OK = 0,			///< everything went fine
		TS_BAD_PASSPHRASE = 1,		///< the passphrase was not correct
		TS_MSG_SEQUENCE = 2,		///< unexpected sequence of GnuPG messages
		TS_USER_ABORTED = 3,		///< the user aborted the transaction
		TS_INVALID_EMAIL = 4,		///< the given email address is invalid
		TS_INPUT_PROCESS_ERROR = 5,	///< the connected input process returned an error
		TS_COMMON_END = 100		///< placeholder for return values of derived classes
	};
	/**
	 * @brief result codes for GnuPG boolean questions
	 *
	 * These are the possible answers to a boolean question of a GnuPG process.
	 */
	enum ts_boolanswer {
		BA_UNKNOWN = 0,			///< the question is not supported (this is an error)
		BA_YES = 1,			///< answer "YES"
		BA_NO = 2			///< answer "NO"
	};
	/**
	 * @brief actions to do after a passphrase was sent to GnuPG
	 */
	enum ts_passphrase_actions {
		PA_NONE = 0,			///< do nothing special
		PA_CLOSE_GOOD = 1,		///< close command channel if passphrase was accepted
		PA_USER_ABORTED = 2		///< the user has cancelled the dialog, abort the transaction
	};

	/**
	 * @brief KGpgTransaction constructor
	 */
	explicit KGpgTransaction(QObject *parent = 0, const bool allowChaining = false);
	/**
	 * @brief KGpgTransaction destructor
	 */
	virtual ~KGpgTransaction();

	/**
	 * @brief Start the operation.
	 */
	void start();

	/**
	 * @brief sets the home directory of GnuPG called for this transaction
	 */
	void setGnuPGHome(const QString &home);

	/**
	 * @brief blocks until the transaction is complete
	 * @return the result of the transaction like done() would
	 * @retval TS_USER_ABORTED the timeout expired
	 *
	 * If this transaction has another transaction set as input then
	 * it would wait for those transaction to finish first. The msecs
	 * argument is used as limit for both transactions then so you
	 * can end up waiting twice the given time (or longer if you have
	 * more transactions chained).
	 */
	int waitForFinished(const int msecs = -1);

	/**
	 * @brief return description of this transaction
	 * @return string used to describe what's going on
	 *
	 * This is especially useful when using this transaction from a KJob.
	 */
	const QString &getDescription() const;

	/**
	 * @brief connect the standard input of this transaction to another process
	 * @param proc process to read data from
	 *
	 * Once the input process is connected this transaction will not emit
	 * the done signal until the input process sends the done signal.
	 *
	 * The basic idea is that when an input transaction is set you only need
	 * to care about this transaction. The other transaction is automatically
	 * started when this one is started and is destroyed when this one is.
	 */
	void setInputTransaction(KGpgTransaction *ta);

	/**
	 * @brief tell the process the standard input is no longer connected
	 *
	 * If you had connected an input process you need to tell the transaction
	 * once this input process is gone. Otherwise you will not get a done
	 * signal from this transaction as it will wait for the finished signal
	 * from the process that will never come.
	 */
	void clearInputTransaction();

	/**
	 * @brief check if another transaction will sent input to this
	 */
	bool hasInputTransaction() const;

	/**
	 * @brief abort this operation as soon as possible
	 */
	void kill();

signals:
	/**
	 * @brief Emitted when the operation was completed.
	 * @param result return status of the transaction
	 *
	 * @see ts_transaction for the common status codes. Each transaction
	 * may define additional status codes.
	 */
	void done(int result);

	/**
	 * @brief emits textual status information
	 * @param msg the status message
	 */
	void statusMessage(const QString &msg);

	/**
	 * @brief emits procentual status information
	 * @param processedAmount how much of the job is done
	 * @param totalAmount how much needs to be done to complete this job
	 */
	void infoProgress(qulonglong processedAmount, qulonglong totalAmount);

protected:
	/**
	 * @brief Called before the gpg process is started.
	 * @return true if the process should be started
	 *
	 * You may reimplement this member if you need to do some special
	 * operations before the process is started. The command line of the
	 * process may be modified for the last time here.
	 *
	 * When you notice that some values passed are invalid or the
	 * transaction does not need to be run for some other reason you should
	 * call setSuccess() to set the return value and return false. In this
	 * case the process is not started but the value is immediately
	 * returned.
	 */
	virtual bool preStart();
	/**
	 * @brief Called when the gpg process is up and running.
	 *
	 * This functions is connected to the started() signal of the gpg process.
	 */
	virtual void postStart();
	/**
	 * @brief Called for every line the gpg process writes.
	 * @param line the input from the process
	 * @return true if "quit" should be sent to process
	 *
	 * You need to implement this member to get a usable subclass.
	 *
	 * When this function returns true "quit" is written to the process.
	 */
	virtual bool nextLine(const QString &line) = 0;
	/**
	 * @brief Called for every boolean question GnuPG answers
	 * @param line the question GnuPG asked
	 * @return what to answer GnuPG
	 *
	 * This is called instead of nextLine() if the line contains a boolean
	 * question. Returning BA_UNKNOWN will cancel the current transaction
	 * and will set the transaction result to TS_MSG_SEQUENCE.
	 *
	 * The default implementation will answer BA_UNKNOWN to every question.
	 */
	virtual ts_boolanswer boolQuestion(const QString &line);
	/**
	 * @brief Called when the gpg process finishes.
	 *
	 * You may reimplement this member if you need to do some special
	 * operations after process completion. The provided one simply
	 * does nothing which should be enough for most cases.
	 */
	virtual void finish();
	/**
	 * @brief called when the user entered a new password
	 *
	 * This is called after askNewPassphrase() was called, the user has
	 * entered a new password and it was sent to the GnuPG process.
	 *
	 * The default implementation does nothing.
	 */
	virtual void newPasswordEntered();
	/**
	 * @brief set the description returned in getDescription()
	 * @param description the new description of this transaction
	 */
	void setDescription(const QString &description);

	/**
	 * @brief wait until the input transaction has finished
	 */
	void waitForInputTransaction();

	/**
	 * @brief notify of an unexpected line
	 *
	 * This will print out the line to the console to ease debugging.
	 */
	void unexpectedLine(const QString &line);

	/**
	 * @brief called when GnuPG asks for a passphrase
	 * @return true if "quit" should be sent to process
	 *
	 * This allows a transaction to implement special handling for
	 * passphrases, e.g. when both old and new passphrase must be
	 * requested when changing it. The default implementation will just
	 * call askPassphrase().
	 */
	virtual ts_passphrase_actions passphraseRequested();

private:
	KGpgTransactionPrivate* const d;

	Q_PRIVATE_SLOT(d, void slotReadReady())
	Q_PRIVATE_SLOT(d, void slotProcessExited())
	Q_PRIVATE_SLOT(d, void slotProcessStarted())
	Q_PRIVATE_SLOT(d, void slotInputTransactionDone(int))
	Q_PRIVATE_SLOT(d, void slotPasswordEntered(const QString &))
	Q_PRIVATE_SLOT(d, void slotPasswordAborted())

protected:
	/**
	 * @brief Ask user for passphrase and send it to gpg process.
	 *
	 * If the gpg process asks for a new passphrase this function will do
	 * all necessary steps for you: ask the user for the password and write
	 * it to the gpg process. If the password is wrong the user is prompted
	 * again for the correct password. If the user aborts the password
	 * entry the gpg process will be killed and the transaction result will
	 * be set to TS_USER_ABORTED.
	 *
	 * In contrast to sendPassphrase() this function will not block, but
	 * handle everything using signals and slots.
	 *
	 * @see KgpgInterface::sendPassphrase
	 * @see askPassphrase
	 */
	void askNewPassphrase(const QString &text);

	/**
	 * @brief get the success value that will be returned with the done signal
	 */
	int getSuccess() const;
	/**
	 * @brief set the success value that will be returned with the done signal
	 * @param v the new success value
	 *
	 * You should use 0 as success value. Other values can be defined as needed.
	 */
	void setSuccess(const int v);

	/**
	 * @brief add a userid hint
	 * @param txt userid description
	 *
	 * Before GnuPG asks for a passphrase it usually sends out a hint message
	 * for which key the passphrase will be needed. There may be several hint
	 * messages, e.g. if a text was encrypted with multiple keys.
	 */
	void addIdHint(QString txt);
	/**
	 * @brief get string of all userid hints
	 * @returns concatenation of all ids previously added with addIdHint().
	 */
	QString getIdHints() const;

	/**
	 * @brief get a reference to the gpg process object
	 * @returns gpg process object
	 *
	 * This returns a reference to the gpg process object used internally.
	 * In case you need to do some special things (e.g. changing the output
	 * mode) you can modify this object.
	 *
	 * Usually you will not need this.
	 *
	 * @warning Never free this object!
	 */
	GPGProc *getProcess();
	/**
	 * @brief add a command line argument to gpg process
	 * @param arg new argument
	 * @returns the position of the new argument
	 *
	 * This is a convenience function that allows adding one additional
	 * argument to the command line of the process. This must be called
	 * before start() is called. Usually you will call this from your
	 * constructor.
	 */
	int addArgument(const QString &arg);
	/**
	 * @brief add command line arguments to gpg process
	 * @param args new arguments
	 * @returns the position of the first argument added
	 *
	 * This is a convenience function that allows adding additional
	 * arguments to the command line of the process. This must be called
	 * before start() is called.
	 */
	int addArguments(const QStringList &args);
	/**
	 * @brief replace the argument at the given position
	 * @param pos position of old argument
	 * @param arg new argument
	 */
	void replaceArgument(const int pos, const QString &arg);
	/**
	 * @brief insert an argument at the given position
	 * @param pos position to insert at
	 * @param arg new argument
	 */
	void insertArgument(const int pos, const QString &arg);
	/**
	 * @brief insert arguments at the given position
	 * @param pos position to insert at
	 * @param args new arguments
	 */
	void insertArguments(const int pos, const QStringList &args);
	/**
	 * @brief make sure the reference to a specific argument is kept up to date
	 * @param ref the value where the position is stored
	 *
	 * You might want to keep the position of a specific argument to
	 * later be able to repace it easily. In that case put it into
	 * this function too so every time someone mofifies the argument
	 * list (especially by insertArgument() and insertArguments())
	 * this reference will be kept up to date.
	 */
	void addArgumentRef(int *ref);
	/**
	 * @brief write data to standard input of gpg process
	 * @param a data to write
	 * @param lf if line feed should be appended to message
	 *
	 * Use this function to interact with the gpg process. A carriage
	 * return is appended to the data automatically. Usually you will
	 * call this function from nextLine().
	 */
	void write(const QByteArray &a, const bool lf = true);
	/**
	 * @brief write data to standard input of gpg process
	 * @param i data to write
	 *
	 * @overload
	 */
	void write(const int i);
	/**
	 * @brief ask user for password
	 * @param message message to display to the user. If message is empty
	 * "Enter password for [UID]" will be used.
	 * @return true if the authorization was successful
	 *
	 * This function handles user authorization for key changes. It will
	 * take care to display the message asking the user for the password
	 * and the number of tries left.
	 */
	 bool askPassphrase(const QString &message = QString());
};

class KGpgTransactionPrivate: QObject {
public:
    KGpgTransactionPrivate(KGpgTransaction *parent, bool allowChaining);
    ~KGpgTransactionPrivate();

    KGpgTransaction *m_parent;
    GPGProc *m_process;
    KGpgTransaction *m_inputTransaction;
    //KNewPasswordDialog *m_passwordDialog;
    int m_success;
    int m_tries;
    QString m_description;
    bool m_chainingAllowed;
    KGpgTransaction::ts_passphrase_actions m_passphraseAction;	///< ignore further request for passphrase

    QStringList m_idhints;

    void slotReadReady();
    void slotProcessExited();
    void slotProcessStarted();
    void slotInputTransactionDone(int result);
    void slotPasswordEntered(const QString &password);
    void slotPasswordAborted();

    QList<int *> m_argRefs;
    bool m_inputProcessDone;
    int m_inputProcessResult;
    bool m_ownProcessFinished;

    /**
     * terminate GnuPG session
     */
    void sendQuit(void);

    void write(const QByteArray &a);

private:
    void processDone();

    unsigned int m_quitTries;	///< how many times we tried to quit
    QStringList m_quitLines;	///< what we received after we tried to quit
};

#endif // KGPGTRANSACTION_H
