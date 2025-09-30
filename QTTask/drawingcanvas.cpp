#include "drawingcanvas.h"


/**
 * @brief Konstruktor untuk DrawingCanvas. Mengatur kebijakan ukuran dan latar belakang.
 */
DrawingCanvas::DrawingCanvas(QWidget *parent) : QWidget(parent) {
    // Mengatur latar belakang
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(200,200,200));
    setPalette(pal);
    setMinimumSize(400, 400);
}

/**
 * @brief Mengatur hasil Convex Hull yang akan digambar di canvas.
 */
void DrawingCanvas::setConvexHullResult(const QList<QPointF>& hull, int iterations) {
    m_convexHull = hull;
    m_iterationCount = iterations;
    update(); // Meminta pengecatan ulang untuk menampilkan hasil baru
}

/**
 * @brief Menghapus semua titik, garis kuas, dan hasil hull.
 */
Q_SLOT void DrawingCanvas::clearCanvas() {
    m_points.clear();
    m_brushPaths.clear();
    m_convexHull.clear();
    m_iterationCount = 0;
    update(); // Pengecatan ulang untuk membersihkan tampilan
}

/**
 * @brief Menangani saat tombol mouse ditekan.
 */
void DrawingCanvas::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDrawing = true;
        m_lastPos = event->position();

        // Memulai jalur kuas baru untuk gambar bebas
        m_brushPaths.append(QVector<QPointF>());
        m_brushPaths.last().append(m_lastPos);

        // Hapus hasil hull sebelumnya saat user mulai menggambar baru
        m_convexHull.clear();
        m_iterationCount = 0;
        update();
    }
}

/**
 * @brief Menangani pergerakan mouse saat tombol ditekan (menggambar kuas).
 */
void DrawingCanvas::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) && m_isDrawing) {
        // Hanya tambahkan titik jika mouse bergerak cukup jauh untuk menghindari noise
        if (QLineF(m_lastPos, event->position()).length() > 2.0) {
            m_brushPaths.last().append(event->position());
            m_lastPos = event->position();
            update();
        }
    }
}

/**
 * @brief Menangani saat tombol mouse dilepas.
 */
void DrawingCanvas::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDrawing = false;

        // Periksa apakah ini adalah klik cepat (bukan geseran yang signifikan)
        if (m_brushPaths.last().size() == 1 ||
            QLineF(m_brushPaths.last().first(), m_brushPaths.last().last()).length() < CLICK_DISTANCE_THRESHOLD)
        {
            // Hapus jalur kuas yang baru saja dibuat (karena itu adalah klik cepat)
            QPointF point = m_brushPaths.last().first();
            m_brushPaths.removeLast();

            // Tambahkan sebagai titik Convex Hull (titik merah)
            m_points.append(point);

        } else {
            // Ini adalah jalur kuas yang sah, biarkan di m_brushPaths
        }

        update();
    }
}

/**
 * @brief Menangani semua proses pengecatan di canvas.
 */
void DrawingCanvas::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // --- 1. Gambar Gambar Kuas Bebas (Jalur Merah) ---
    QPen brushPen(Qt::red, 2);
    brushPen.setCapStyle(Qt::RoundCap);
    painter.setPen(brushPen);
    for (const auto& path : m_brushPaths) {
        if (path.size() > 1) {
            // Gambar jalur dari titik ke titik
            for (int i = 0; i < path.size() - 1; ++i) {
                painter.drawLine(path[i], path[i+1]);
            }
        }
    }

    // --- 2. Gambar Convex Hull (Garis Hijau) ---
    if (!m_convexHull.isEmpty()) {
        QPen hullPen(Qt::green, 3);
        hullPen.setStyle(Qt::SolidLine);
        painter.setPen(hullPen);
        painter.setBrush(Qt::NoBrush);

        // Gambar garis yang menghubungkan titik-titik hull
        for (int i = 0; i < m_convexHull.size(); ++i) {
            QPointF p1 = m_convexHull[i];
            QPointF p2 = m_convexHull[(i + 1) % m_convexHull.size()]; // Loop kembali ke titik pertama
            painter.drawLine(p1, p2);
        }

        // --- 3. Tampilkan Jumlah Iterasi ---
        QPen textPen(Qt::black);
        painter.setPen(textPen);
        QFont font = painter.font();
        font.setPointSize(14);
        painter.setFont(font);

        QString text = QString("Iterasi: %1").arg(m_iterationCount);
        painter.drawText(10, 30, text); // Gambar di pojok kiri atas
    }

    // --- 4. Gambar Titik Convex Hull ---
    QPen pointPen(Qt::red, 1);
    painter.setPen(pointPen);
    painter.setBrush(Qt::red);
    const float pointSize = 5.0f;

    for (const QPointF& p : m_points) {
        // Gambar kotak kecil atau lingkaran untuk merepresentasikan titik
        painter.drawRect(p.x() - pointSize/2, p.y() - pointSize/2, pointSize, pointSize);
    }
}
