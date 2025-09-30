#include "drawingwindow.h"
#include <algorithm>
#include <utility>

/**
 * @brief Konstruktor untuk DrawingWindow. Mengatur tata letak Jendela Utama.
 */
DrawingWindow::DrawingWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Computer Graphics Simulation"); // Judul

    // --- Inisialisasi Widgets ---
    m_canvas = new DrawingCanvas(this);
    m_slowButton = new QPushButton("Slow Convex Hull", this);
    m_efficientButton = new QPushButton("Convex Hull Algoritma", this);
    m_clearButton = new QPushButton("Clear Canvas", this);

    // --- Styling Tombol (Putih dengan Teks Hitam) ---
    QString buttonStyle = R"(
        QPushButton {
            background-color: #FFFFFF;
            color: #000000;
            border: 1px solid #CCCCCC;
            border-radius: 5px;
            padding: 8px 15px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #EEEEEE;
        }
        QPushButton:pressed {
            background-color: #DDDDDD;
        }
    )";

    m_slowButton->setStyleSheet(buttonStyle);
    m_efficientButton->setStyleSheet(buttonStyle);
    m_clearButton->setStyleSheet(buttonStyle);

    // --- Layout Tombol (Horizontal) ---
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_slowButton);
    buttonLayout->addWidget(m_efficientButton);
    buttonLayout->addWidget(m_clearButton);

    // --- Layout Utama (Vertikal: Canvas di atas, Tombol di bawah) ---
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_canvas, 1); // 1 = Canvas mengambil sebagian besar ruang
    mainLayout->addLayout(buttonLayout, 0); // 0 = Tombol mengambil ruang minimum yang diperlukan

    // Mengatur margin agar tata letak tidak terlalu rapat dengan tepi jendela
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10); // Spasi antara canvas dan tombol

    // --- Koneksi Sinyal dan Slot ---
    connect(m_slowButton, &QPushButton::clicked, this, &DrawingWindow::on_slowConvexHullButton_clicked);
    connect(m_efficientButton, &QPushButton::clicked, this, &DrawingWindow::on_convexHullAlgorithmButton_clicked);
    connect(m_clearButton, &QPushButton::clicked, this, &DrawingWindow::on_clearCanvasButton_clicked);
}

/**
 * @brief Menghitung orientasi tiga titik (p1, p2, p3).
 * Menggunakan cross product 2D (z-component dari (p2-p1) x (p3-p1)).
 * Nilai > 0: Belok Kiri (Counter-Clockwise)
 * Nilai < 0: Belok Kanan (Clockwise)
 * Nilai = 0: Segaris (Collinear)
 */
qreal DrawingWindow::calculateOrientation(const QPointF& p1, const QPointF& p2, const QPointF& p3) {
    // Rumus: (y2 - y1) * (x3 - x2) - (x2 - x1) * (y3 - y2)
    qreal val = (p2.y() - p1.y()) * (p3.x() - p2.x()) -
                (p2.x() - p1.x()) * (p3.y() - p2.y());
    return val;
}

// ALGORITMA 1: SLOW CONVEX HULL

void DrawingWindow::on_slowConvexHullButton_clicked() {
    QVector<QPointF> points = m_canvas->getPoints();
    if (points.size() < 3) {
        qDebug() << "Minimal 3 titik diperlukan untuk Convex Hull.";
        m_canvas->setConvexHullResult({}, 0);
        return;
    }

    QList<QPointF> hull;
    QList<QLineF> edges;
    int iterations = 0;

    int n = points.size();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;

            const QPointF& p = points[i];
            const QPointF& q = points[j];
            bool valid = true;

            for (int k = 0; k < n; ++k) {
                if (k == i || k == j) continue;

                const QPointF& r = points[k];
                iterations++;

                qreal orientation = calculateOrientation(p, q, r);

                if (orientation > 0) {
                    valid = false;
                    break;
                }
            }

            if (valid) {
                edges.append(QLineF(p, q));
            }
        }
    }


    if (!edges.isEmpty()) {
        QPointF startPoint = edges.first().p1();
        QPointF currentPoint = startPoint;

        do {
            hull.append(currentPoint);
            bool foundNext = false;
            for (const QLineF& edge : std::as_const(edges)) {
                if (edge.p1() == currentPoint) {
                    currentPoint = edge.p2();
                    foundNext = true;
                    break;
                }
            }
            if (!foundNext) break;
        } while (currentPoint != startPoint && hull.size() <= points.size());

        if (currentPoint != startPoint || hull.size() < 3) {
            hull.clear();
        }
    }


    m_canvas->setConvexHullResult(hull, iterations);
    qDebug() << "Slow Convex Hull selesai. Iterasi:" << iterations;
}


// ALGORITMA 2: CONVEX HULL EFFICIENT

QList<QPointF> DrawingWindow::monotoneChainConvexHull(const QVector<QPointF>& points, int& iterations) {
    iterations = 0;
    int n = points.size();
    if (n <= 3) return points.toList();

    QVector<QPointF> sortedPoints = points;
    std::sort(sortedPoints.begin(), sortedPoints.end(), [](const QPointF& a, const QPointF& b) {
        if (a.x() != b.x())
            return a.x() < b.x();
        return a.y() < b.y();
    });

    QList<QPointF> upperHull;
    QList<QPointF> lowerHull;

    for (int i = 0; i < n; ++i) {
        iterations++;
        while (upperHull.size() >= 2 && calculateOrientation(upperHull[upperHull.size() - 2], upperHull.last(), sortedPoints[i]) <= 0) {
            upperHull.removeLast();
            iterations++;
        }
        upperHull.append(sortedPoints[i]);
    }

    for (int i = n - 1; i >= 0; --i) {
        iterations++;
        while (lowerHull.size() >= 2 && calculateOrientation(lowerHull[lowerHull.size() - 2], lowerHull.last(), sortedPoints[i]) <= 0) {
            lowerHull.removeLast();
            iterations++;
        }
        lowerHull.append(sortedPoints[i]);
    }

    QList<QPointF> hull;
    hull.reserve(upperHull.size() + lowerHull.size() - 2);

    hull.append(upperHull);

    for (int i = 1; i < lowerHull.size() - 1; ++i) {
        hull.append(lowerHull[i]);
    }

    return hull;
}

void DrawingWindow::on_convexHullAlgorithmButton_clicked() {
    QVector<QPointF> points = m_canvas->getPoints();
    if (points.size() < 3) {
        qDebug() << "Minimal 3 titik diperlukan untuk Convex Hull.";
        m_canvas->setConvexHullResult({}, 0);
        return;
    }

    int iterations = 0;
    QList<QPointF> hull = monotoneChainConvexHull(points, iterations);

    m_canvas->setConvexHullResult(hull, iterations);
    qDebug() << "Efficient Convex Hull selesai. Iterasi:" << iterations;
}


/**
 * @brief Slot untuk tombol Clear Canvas.
 */
void DrawingWindow::on_clearCanvasButton_clicked() {
    m_canvas->clearCanvas();
    qDebug() << "Canvas dibersihkan.";
}
