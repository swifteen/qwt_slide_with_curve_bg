/*****************************************************************************
 * Qwt Examples
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "SliderBox.h"

#include <qwt_scale_engine.h>
#include <qwt_date.h>
#include <qwt_date_scale_draw.h>
#include <qwt_date_scale_engine.h>
#include <qwt_transform.h>
#include <qwt_painter.h>
#include <qwt_plot_layout.h>
#include <QwtPlot>
#include <QwtPlotCurve>
#include <QwtSymbol>
#include <QwtScaleMap>
#include <QwtPlotDirectPainter>
#include <QwtPainter>
#include <QwtPlotRenderer>

#include <qlabel.h>
#include <qlayout.h>
#include <QPainter>
#include <QDebug>
#include <QResizeEvent>
#include <QPalette>
namespace
{
class CurveData : public QwtArraySeriesData< QPointF >
{
public:
    virtual QRectF boundingRect() const QWT_OVERRIDE
    {
        if ( cachedBoundingRect.width() < 0.0 )
            cachedBoundingRect = qwtBoundingRect( *this );

        return cachedBoundingRect;
    }

    inline void append( const QPointF& point )
    {
        m_samples += point;
    }

    void clear()
    {
        m_samples.clear();
        m_samples.squeeze();
        cachedBoundingRect = QRectF( 0.0, 0.0, -1.0, -1.0 );
    }
};
}

SliderBox::SliderBox(QWidget *parent ):
    QwtSlider( parent ),
    m_directPainter(nullptr),
    m_curve(nullptr),
    m_plot(nullptr)
{
    m_lastUpdateTimestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setOrientation( Qt::Horizontal );
    setScalePosition( QwtSlider::LeadingScale );
    setTrough( true );
    setGroove( false );
    setSpacing(0);//修改刻度标尺与滑块的间隔
    setHandleSize( QSize( 20, 32 ) );//设置滑块size

    QMargins margins = contentsMargins();
    margins.setTop(10);
    setContentsMargins(margins);
    setBorderWidth( 1 );

    setScaleMaxMinor( 9 );
    setStepAlignment( false );

    // 设置滑块颜色
    QPalette palette1 = palette();
    palette1.setColor(QPalette::Button, Qt::darkGreen); // 设置滑块颜色为红色
    setPalette(palette1);


    initPlot();
    initScale();
}

SliderBox::~SliderBox()
{
    delete m_directPainter;
    delete m_plot;
}

void SliderBox::initScale()
{
    QwtDateScaleDraw *scaleDraw = new QwtDateScaleDraw( Qt::LocalTime );
    scaleDraw->setDateFormat(QwtDate::Millisecond,"yyyyMMdd hh:mm:ss:zzz");
    scaleDraw->setDateFormat(QwtDate::Second,"yyyyMMdd hh:mm:ss");
    scaleDraw->setDateFormat(QwtDate::Minute,"yyyyMMdd hh:mm");
    scaleDraw->setDateFormat(QwtDate::Hour,"yyyyMMdd hh:mm");
    scaleDraw->setDateFormat(QwtDate::Day,"yyyyMMdd");
    scaleDraw->setDateFormat(QwtDate::Week,"yyyy Www");
    scaleDraw->setDateFormat(QwtDate::Month,"yyyyMM");
    setScaleDraw( scaleDraw );

    QwtDateScaleEngine *scaleEngine = new QwtDateScaleEngine( Qt::LocalTime );
    setScaleEngine( scaleEngine );

    QDateTime startDateTime = QDateTime::currentDateTime();
    QDateTime endDateTime = startDateTime.addSecs(100);
    setScale( QwtDate::toDouble(startDateTime),QwtDate::toDouble(endDateTime));

    //点击滑块左右两侧控制滑块单次移动的步伐和整体步数
    setTotalSteps( 100 );
    setPageSteps( 12 );
}

void SliderBox::initPlot()
{
    m_plot = new QwtPlot();
    qDebug()<<__LINE__<<m_plot->canvas()->size();
    //参考QwtSlider::drawSlider中获取Trough的区域
    const int bw = borderWidth();
    QRect innerRect = sliderRect().adjusted( bw, bw, -bw, -bw );
    m_plot->canvas()->resize(innerRect.size());//将波形区域设置成和Trough尺寸一样
    qDebug()<<__LINE__<<m_plot->canvas()->size();
    m_directPainter = new QwtPlotDirectPainter( m_plot );
    m_curve = new QwtPlotCurve( "Test Curve" );
    m_curve->setRenderHint( QwtPlotItem::RenderAntialiased );
    QColor c = Qt::red;
    c.setAlpha( 150 );
    m_curve->setPen( QPen( Qt::NoPen ) );
    m_curve->setBrush( c );//设置画刷得到曲线背景效果
    m_curve->setData( new CurveData() );
    m_curve->attach( m_plot );
    m_plot->setAutoReplot( false );
    m_plot->setAxisScale(QwtPlot::yLeft, 0.0, 350.0);
    m_plot->setAxisAutoScale(QwtPlot::yLeft, true);//自动设置Y轴范围
    m_plot->setAxisVisible(QwtPlot::xBottom, false);
    m_plot->setAxisVisible(QwtPlot::yLeft, false);
    m_plot->plotLayout()->setAlignCanvasToScales( true );
    m_plot->plotLayout()->setCanvasMargin( 0 );
    m_plot->setFrameStyle( QFrame::NoFrame );
    m_plot->setLineWidth( 0 );
    m_plot->replot();
}

void SliderBox::resizeEvent( QResizeEvent* event )
{
    m_directPainter->reset();

    if(m_plot)
    {
        qDebug()<<__LINE__<<m_plot->canvas()->size()<< event->size() ;
        //参考QwtSlider::drawSlider中获取Trough的区域
        const int bw = borderWidth();
        QRect innerRect = sliderRect().adjusted( bw, bw, -bw, -bw );
        m_plot->canvas()->resize(innerRect.size());
        qDebug()<<__LINE__<<m_plot->canvas()->size()<< event->size()<<innerRect ;
    }

    QwtSlider::resizeEvent( event );
}

void SliderBox::paintEvent( QPaintEvent* event )
{
//    qDebug()<<__LINE__<<sliderRect()<<handleRect();
    QwtSlider::paintEvent(event );
    QPainter painter(this);
    // 创建一个QwtPlotRenderer对象，并设置其画布大小和分辨率
    QwtPlotRenderer renderer;
    renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, false);
    renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasBackground, false);
    renderer.setLayoutFlag(QwtPlotRenderer::FrameWithScales);
    //    painter.setOpacity(0.5);//设置透明度
    const int bw = borderWidth();
    QRect innerRect = sliderRect().adjusted( bw, bw, -bw, -bw );
    renderer.render(m_plot,&painter, innerRect);

    if ( isValid() )
        drawHandle( &painter, handleRect(), transform( value() ) );
    painter.end();
}

void SliderBox::drawSlider(QPainter *painter, const QRect& rect) const
{
    // 调用父类的绘制函数
    QwtSlider::drawSlider(painter,rect);

    // 在滑块上绘制当前值
    painter->save();
    QFont font = painter->font();
    font.setBold( true );
    painter->setFont(font);
    //    painter->setPen(Qt::blue);  // 设置颜色为红色
    const int labelWidth = QwtPainter::horizontalAdvance(QFontMetrics(painter->font()), "yyyyMMdd HH:mm:ss" );
    if(handleRect().center().x() > labelWidth + labelWidth/2)
    {
        QString startDataTimeStr = QwtDate::toDateTime( lowerBound(), Qt::LocalTime ).toString("yyyyMMdd HH:mm:ss");
        painter->drawText(sliderRect().topLeft(), startDataTimeStr);
    }

    if(handleRect().center().x() < sliderRect().right() - (labelWidth + labelWidth/2))
    {
        QString endDataTimeStr = QwtDate::toDateTime( upperBound(), Qt::LocalTime ).toString("yyyyMMdd HH:mm:ss");
        QPoint startPoint = QPoint(sliderRect().right() - labelWidth,handleRect().top());
        painter->drawText(startPoint, endDataTimeStr);
    }
    painter->restore();
}

void SliderBox::drawHandle( QPainter* painter, const QRect& rect, int pos ) const
{
    // 调用父类的绘制函数
    QwtSlider::drawHandle(painter,rect,pos);

    // 在滑块上绘制当前值
    painter->save();
    QFont font = painter->font();
    font.setBold( true );
    painter->setFont(font);
    //    painter->setPen(Qt::blue);  // 设置颜色为红色
    QString curDataTimeStr = QwtDate::toDateTime( value(), Qt::LocalTime ).toString("yyyyMMdd HH:mm:ss");
    const int labelWidth = QwtPainter::horizontalAdvance(QFontMetrics(painter->font()), curDataTimeStr );
    QPoint startPoint;
    int handleCenterX = handleRect().center().x();
    if((handleCenterX > labelWidth/2) && (handleCenterX + labelWidth/2 < sliderRect().right()))
    {
        startPoint = QPoint(handleRect().left() - labelWidth/2,handleRect().top());
    }
    else if(handleCenterX <= labelWidth/2)
    {
        startPoint = QPoint(sliderRect().left(),handleRect().top());
    }
    else
    {
        startPoint = QPoint(sliderRect().right() - labelWidth,handleRect().top());
    }
    painter->drawText(startPoint, curDataTimeStr);
    painter->restore();
}

void SliderBox::scaleChange()
{
    qDebug()<<__LINE__<<lowerBound()<<upperBound();
    if(m_plot)
    {
        m_plot->setAxisScale(QwtPlot::xBottom, lowerBound(),upperBound());
        m_plot->replot();
    }
    QwtSlider::scaleChange();
}

void SliderBox::append_point( const QPointF& point )
{
    CurveData* curveData = static_cast< CurveData* >( m_curve->data() );
    curveData->append( point );
    m_directPainter->drawSeries( m_curve,curveData->size() - 1, curveData->size() - 1 );
    //通过时间间隔定时更新进度
    qint64 cur_timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (cur_timestamp - m_lastUpdateTimestamp > 30)
    {
        update();
        m_lastUpdateTimestamp = cur_timestamp;
    }
}

void SliderBox::clear_points()
{
    CurveData* curveData = static_cast< CurveData* >( m_curve->data() );
    curveData->clear();

    m_plot->replot();
}

