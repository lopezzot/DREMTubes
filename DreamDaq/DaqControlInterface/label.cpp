#include "label.h"

Label::Label(QWidget *parent) :
    QLabel(parent)
{
    str1="Label";
    str2="label";
    status=true;
}

void Label::switchText(){
    this->setText((text().compare(str1)? str1 : str2));
    status=!status;
}
void Label::setStrings(QString s1, QString s2){
    str1=s1;
    str2=s2;
    status=true;
    this->setText(s1);
}

void Label::mouseReleaseEvent(QMouseEvent *ev){
    if (ev->button()==Qt::LeftButton) {
        ev->accept();
        status=!status;
        emit clicked();
    }
    else ev->ignore();
    if (status) {
        this->setText(str1);
    }
    else {
        this->setText(str2);
    }
}
