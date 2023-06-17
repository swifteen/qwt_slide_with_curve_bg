#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class SliderBox;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
protected:

  virtual void timerEvent( QTimerEvent* );

private:
    Ui::Widget *ui;
    SliderBox *d_sliderBox;
    int m_timerId;

};
#endif // WIDGET_H
