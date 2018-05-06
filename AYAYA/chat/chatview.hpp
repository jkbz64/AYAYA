#ifndef CHATVIEW_HPP
#define CHATVIEW_HPP

#include <QTextBrowser>

class ChatView : public QTextBrowser {
    Q_OBJECT
public:
    ChatView(QWidget* = nullptr);
    virtual ~ChatView();

    void reset();
    void addMessage(const QString&);

    void setSpacing(int);
    int spacing() const;

protected:
    int m_spacing;
};

#endif // CHATVIEW_HPP
