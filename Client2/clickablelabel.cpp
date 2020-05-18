#include "ClickableLabel.h"

//*********************************************************************
//
//Questa classe rende un oggetto "label" cliccabile ed abilita
//gli eventi di hoverEnter e Leave
//
//*********************************************************************

ClickableLabel::ClickableLabel(QWidget* parent)
    : QLabel(parent) {

    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);
}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked();
}

void ClickableLabel::hoverEnter(QHoverEvent* ){
    emit hovered();
}

void ClickableLabel::hoverLeave(QHoverEvent* ){
    emit unHovered();
}

bool ClickableLabel::event(QEvent *event){

        switch(event->type())
        {
        case QEvent::HoverEnter:
            hoverEnter(static_cast<QHoverEvent*>(event));
            return true;
            break;
        case QEvent::HoverLeave:
            hoverLeave(static_cast<QHoverEvent*>(event));
            return true;
            break;
        default:
            break;
        }
        return QWidget::event(event);

}
