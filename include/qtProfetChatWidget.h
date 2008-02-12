#ifndef QTPROFETCHATWIDGET_H_
#define QTPROFETCHATWIDGET_H_

#include <q3vbox.h>
#include <profet/ProfetCommon.h>


class Q3TextEdit;
class QLineEdit;
class QPushButton;

class ProfetChatWidget : public Q3VBox {
  Q_OBJECT
private:
  Q3TextEdit * textEdit;
  QLineEdit * lineEdit;
  QPushButton * sendButton;
  
public:
	ProfetChatWidget(QWidget * parent);
	virtual ~ProfetChatWidget(); 
	/**
   * Display a message
   * @param msg The message to display
   */
  void showMessage(const Profet::InstantMessage & msg);
  /**
   * Notify the user of an error
   * @param errorMsg The error information
   */
  void showErrorMessage(const QString & message);
  /**
   * Set GUI in connected/disconnected mode
   * @param connected mode
   */
  void setConnected(bool connected);

private slots:
  void sendMessagePerformed();
  
signals:
  void sendMessage(const QString & message);
  
};

#endif /*QTPROFETCHATWIDGET_H_*/
