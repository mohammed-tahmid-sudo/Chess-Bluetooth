// Qt drag & drop between QPushButtons with images

#include <QApplication>
#include <QPushButton>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

class DragButton : public QPushButton {
    Q_OBJECT
public:
    explicit DragButton(const QPixmap &pix, QWidget *parent = nullptr)
        : QPushButton(parent), pixmap(pix) {
        setIcon(QIcon(pixmap));
        setIconSize(pixmap.size());
        setAcceptDrops(true);
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            QDrag *drag = new QDrag(this);
            QMimeData *mime = new QMimeData;

            QByteArray ba;
            QBuffer buffer(&ba);
            buffer.open(QIODevice::WriteOnly);
            pixmap.save(&buffer, "PNG");
            mime->setData("application/x-pixmap", ba);

            drag->setMimeData(mime);
            drag->setPixmap(pixmap);
            drag->exec(Qt::CopyAction | Qt::MoveAction);
        }
        QPushButton::mousePressEvent(event);
    }

    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasFormat("application/x-pixmap"))
            event->acceptProposedAction();
    }

    void dropEvent(QDropEvent *event) override {
        if (event->mimeData()->hasFormat("application/x-pixmap")) {
            QByteArray ba = event->mimeData()->data("application/x-pixmap");
            QPixmap droppedPix;
            droppedPix.loadFromData(ba, "PNG");
            pixmap = droppedPix;
            setIcon(QIcon(pixmap));
            setIconSize(pixmap.size());
            event->acceptProposedAction();
        }
    }

private:
    QPixmap pixmap;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    DragButton btn1(QPixmap(":/images/image1.png"));
    btn1.show();

    DragButton btn2(QPixmap(":/images/image2.png"));
    btn2.move(150, 0);
    btn2.show();

    return app.exec();
}

#include "main.moc"

