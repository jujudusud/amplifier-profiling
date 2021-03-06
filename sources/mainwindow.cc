//          Copyright Jean Pierre Cimalando 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "application.h"
#include "analyzerdefs.h"
#include <qwt_scale_engine.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_legenditem.h>
#include <cmath>

struct MainWindow::Impl {
    Ui::MainWindow ui;
    QwtPlotCurve *curve_lo_mag_ = nullptr;
    QwtPlotCurve *curve_lo_phase_ = nullptr;
    QwtPlotCurve *curve_hi_mag_ = nullptr;
    QwtPlotCurve *curve_hi_phase_ = nullptr;
    QwtPlotMarker *marker_mag_ = nullptr;
    QwtPlotMarker *marker_phase_ = nullptr;
    QwtPlotLegendItem *legend_mag_ = nullptr;
    QwtPlotLegendItem *legend_phase_ = nullptr;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), P(new Impl)
{
    P->ui.setupUi(this);

    for (QwtPlot *plt : {P->ui.pltAmplitude, P->ui.pltPhase}) {
        plt->setAxisScale(QwtPlot::xBottom, Analysis::freq_range_min, Analysis::freq_range_max);
        plt->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);
        plt->setCanvasBackground(Qt::darkBlue);
        QwtPlotGrid *grid = new QwtPlotGrid;
        grid->setPen(Qt::gray, 0.0, Qt::DotLine);
        grid->attach(plt);
    }

    QwtPlotCurve *curve_lo_mag = P->curve_lo_mag_ = new QwtPlotCurve(tr("Lo Signal Gain"));
    curve_lo_mag->attach(P->ui.pltAmplitude);
    curve_lo_mag->setPen(Qt::green, 0.0, Qt::SolidLine);
    QwtPlotCurve *curve_lo_phase = P->curve_lo_phase_ = new QwtPlotCurve(tr("Lo Signal Phase"));
    curve_lo_phase->attach(P->ui.pltPhase);
    curve_lo_phase->setPen(Qt::green, 0.0, Qt::SolidLine);

    QwtPlotCurve *curve_hi_mag = P->curve_hi_mag_ = new QwtPlotCurve(tr("Hi Signal Gain"));
    curve_hi_mag->attach(P->ui.pltAmplitude);
    curve_hi_mag->setPen(Qt::red, 0.0, Qt::SolidLine);
    QwtPlotCurve *curve_hi_phase = P->curve_hi_phase_ = new QwtPlotCurve(tr("Hi Signal Phase"));
    curve_hi_phase->attach(P->ui.pltPhase);
    curve_hi_phase->setPen(Qt::red, 0.0, Qt::SolidLine);

    QwtPlotMarker *marker_mag = P->marker_mag_ = new QwtPlotMarker;
    marker_mag->attach(P->ui.pltAmplitude);
    marker_mag->setLineStyle(QwtPlotMarker::VLine);
    marker_mag->setLinePen(Qt::yellow, 0.0, Qt::DashLine);
    QwtPlotMarker *marker_phase = P->marker_phase_ = new QwtPlotMarker;
    marker_phase->attach(P->ui.pltPhase);
    marker_phase->setLineStyle(QwtPlotMarker::VLine);
    marker_phase->setLinePen(Qt::yellow, 0.0, Qt::DashLine);

    QwtPlotLegendItem *legend_mag = P->legend_mag_ = new QwtPlotLegendItem;
    legend_mag->attach(P->ui.pltAmplitude);
    QwtPlotLegendItem *legend_phase = P->legend_phase_ = new QwtPlotLegendItem;
    legend_phase->attach(P->ui.pltPhase);
    for (QwtPlotLegendItem *x : {legend_mag, legend_phase}) {
        x->setTextPen(QPen(Qt::white));
        x->setBorderPen(QPen(Qt::white));
        QColor bg(Qt::gray);
        bg.setAlpha(160);
        x->setBackgroundBrush(bg);
        x->setAlignment(Qt::AlignRight|Qt::AlignTop);
        x->setMaxColumns(1);
        x->setBackgroundMode(QwtPlotLegendItem::LegendBackground);
        x->setBorderRadius(8);
        x->setMargin(4);
        x->setSpacing(2);
        x->setItemMargin(0);
        QFont f = x->font();
        f.setPointSize(10);
        x->setFont(f);
    }

    P->ui.pltAmplitude->setAxisScale(QwtPlot::yLeft, Analysis::db_range_min, Analysis::db_range_max);
    P->ui.pltPhase->setAxisScale(QwtPlot::yLeft, -M_PI, +M_PI);

    connect(P->ui.btn_startSweep, &QAbstractButton::clicked, theApplication, &Application::setSweepActive);
    connect(P->ui.btn_save, &QAbstractButton::clicked, theApplication, &Application::saveProfile);
}

MainWindow::~MainWindow()
{
}

void MainWindow::showCurrentFrequency(float f)
{
    QString text;
    if (f < 1000)
        text = QString::number(std::lround(f)) + " Hz";
    else
        text = QString::number(std::lround(f * 1e-3)) + " kHz";
    P->ui.lbl_frequency->setText(text);
}

void MainWindow::showLevels(float in, float out)
{
    float g_min = P->ui.vu_input->minimum();
    float a_min = std::pow(10.0f, g_min * 0.05f);
    float g_in = (in > a_min) ? (20 * std::log10(in)) : g_min;
    float g_out = (out > a_min) ? (20 * std::log10(out)) : g_min;
    P->ui.vu_input->setValue(g_in);
    P->ui.vu_output->setValue(g_out);
}

void MainWindow::showProgress(float progress)
{
    P->ui.progressBar->setValue(std::lround(progress * 100));
}

void MainWindow::showPlotData(
    const double *freqs, double freqmark,
    const double *lo_mags, const double *lo_phases,
    const double *hi_mags, const double *hi_phases, unsigned n)
{
    P->curve_lo_mag_->setRawSamples(freqs, lo_mags, n);
    P->curve_lo_phase_->setRawSamples(freqs, lo_phases, n);
    P->curve_hi_mag_->setRawSamples(freqs, hi_mags, n);
    P->curve_hi_phase_->setRawSamples(freqs, hi_phases, n);

    P->marker_mag_->setXValue(freqmark);
    P->marker_phase_->setXValue(freqmark);

    P->ui.pltAmplitude->replot();
    P->ui.pltPhase->replot();
}
