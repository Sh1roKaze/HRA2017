/// \file mybutton.h
/// \author xverny00
/// \brief Declaration of custom variant of QObject and QGraphicsPixmapItem, represents a button

#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

class MyButton : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
 
    /// \brief Constructs my button.
    ///
    /// \param parent Pointer to a parent object.
    explicit MyButton(QObject *parent = 0);
    
    /// \brief Set URL of the button picture.
    ///
    /// \param str1 URL of the button picture.
    void setURL(const char *str1);
    
    /// \brief Mouse press event handler.
    ///
    /// \param event Stores info about the event.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:

    /// \brief Button pressed signal.
    void buttonPressed();

public slots:

private:
    QPixmap normal; ///< Pointer to button picture.
};

#endif // MYBUTTON_H
