#include "widget.h"
#include "./ui_widget.h"
#include "SliderBox.h"
#include <QHBoxLayout>
#include <QTimer>
#include <QDebug>
#include <QwtMath>
#include <qwt_date.h>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    d_sliderBox = new SliderBox();
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(d_sliderBox);
    m_timerId = startTimer( 10 );
}

Widget::~Widget()
{
    delete ui;
}

void Widget::timerEvent( QTimerEvent* event )
{
    if ( event->timerId() == m_timerId )
    {
        QDateTime startDateTime = QwtDate::toDateTime( d_sliderBox->lowerBound(), Qt::LocalTime );//获取滑块的起始时间
        QDateTime endDateTime = QwtDate::toDateTime( d_sliderBox->upperBound(), Qt::LocalTime );//获取滑块的结束时间
        static int count = 0;
        count++;
        QDateTime curDateTime = startDateTime.addMSecs(count*30);
        if(curDateTime > endDateTime)
        {
            qDebug()<<__LINE__<<"stop timer";
            killTimer(m_timerId);
            return;
        }
        double y = ( qwtRand() % 350 );
        d_sliderBox->append_point(QPointF( QwtDate::toDouble(curDateTime), y ) );
        return;
    }

    QWidget::timerEvent( event );
}
