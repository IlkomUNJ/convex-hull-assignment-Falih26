#ifndef DRAWINGWINDOW_H
#define DRAWINGWINDOW_H

#include <QWidget>
#include <QPushButton>
#include "drawingcanvas.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QtMath>
#include <QLineF>

// Memastikan semua kelas Qt diwariskan dari QObject
class DrawingWindow : public QWidget {
    Q_OBJECT

public:
    explicit DrawingWindow(QWidget *parent = nullptr);
    ~DrawingWindow() override = default;

private Q_SLOTS:
    void on_slowConvexHullButton_clicked();
    void on_convexHullAlgorithmButton_clicked();
    void on_clearCanvasButton_clicked();

private:
    qreal calculateOrientation(const QPointF& p1, const QPointF& p2, const QPointF& p3);

    QList<QPointF> monotoneChainConvexHull(const QVector<QPointF>& points, int& iterations);

private:
    // Widgets
    DrawingCanvas *m_canvas;
    QPushButton *m_slowButton;
    QPushButton *m_efficientButton;
    QPushButton *m_clearButton;
};

#endif // DRAWINGWINDOW_H
