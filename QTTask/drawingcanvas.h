#ifndef DRAWINGCANVAS_H
#define DRAWINGCANVAS_H

#include <QWidget>
#include <QPointF>
#include <QVector>
#include <QMouseEvent>
#include <QPainter>
#include <QList>
#include <QPainter>
#include <QDebug>
#include <cmath>


class DrawingCanvas : public QWidget {
    Q_OBJECT

public:
    explicit DrawingCanvas(QWidget *parent = nullptr);

    void setConvexHullResult(const QList<QPointF>& hull, int iterations);

    Q_SLOT void clearCanvas();

    const QVector<QPointF>& getPoints() const { return m_points; }

protected:
    // Event handlers untuk interaksi mouse
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    // Data untuk Titik Convex Hull (Klik Cepat)
    QVector<QPointF> m_points;

    // Data untuk Gambar Kuas Bebas (Tahan dan Geser)
    // Menyimpan daftar jalur, di mana setiap jalur adalah vektor titik
    QList<QVector<QPointF>> m_brushPaths;

    // Status Mouse
    bool m_isDrawing = false;
    QPointF m_lastPos;

    // Data Hasil Convex Hull
    QList<QPointF> m_convexHull;
    int m_iterationCount = 0;

    // Toleransi untuk membedakan klik cepat dari awal geseran kuas
    static constexpr double CLICK_DISTANCE_THRESHOLD = 5.0;
};

#endif // DRAWINGCANVAS_H
