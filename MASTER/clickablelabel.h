#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <QEvent>
#include <QHoverEvent>
#include <Qt>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = 0);
    ~ClickableLabel();


protected:
    void mousePressEvent(QMouseEvent* event);
    void hoverEnter(QHoverEvent *event);
    void hoverLeave(QHoverEvent *event);
    bool event(QEvent *event);

signals:
    void clicked();
    void hovered();
    void unHovered();



};

#endif // CLICKABLELABEL_H
