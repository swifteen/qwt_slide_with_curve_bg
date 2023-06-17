/*****************************************************************************
 * Qwt Examples
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef SLIDER_BOX_H
#define SLIDER_BOX_H

#include <qwidget.h>
#include <qwt_slider.h>
class QLabel;
class QwtPlotDirectPainter;
class QwtPlotCurve;
class QwtPlot;
class SliderBox : public QwtSlider
{
    Q_OBJECT
public:
    SliderBox(QWidget *parent = NULL );
    ~SliderBox();
    void append_point(const QPointF &point);
    void clear_points();
protected:
    virtual void drawSlider ( QPainter*, const QRect& ) const;
    virtual void drawHandle( QPainter*, const QRect&, int pos ) const;
    void resizeEvent(QResizeEvent *event);
    virtual void paintEvent ( QPaintEvent* event);
	virtual void scaleChange();
private :
    void initScale();
    void initPlot();
private :
    QwtPlotDirectPainter* m_directPainter;
    QwtPlotCurve* m_curve;
    QwtPlot* m_plot;
    qint64 m_lastUpdateTimestamp;//上次更新时间戳
};

#endif
