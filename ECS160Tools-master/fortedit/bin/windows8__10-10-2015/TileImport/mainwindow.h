#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QPixmap>
#include <QPoint>
#include <QColor>
#include <QListWidgetItem>

class TileImport;
class TileController;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();
        void fillTileList();

    private slots:
        void on_listWidget_clicked(const QModelIndex &index);
        void on_action_Open_Tileset_triggered();
        void on_actionSave_triggered();
        void on_pushButton_clicked();
        void on_animateButton_clicked();

        void mousePressEvent(QMouseEvent* mouseEvent);
        void mouseReleaseEvent(QMouseEvent* mouseEvent);
        void mouseMoveEvent(QMouseEvent* mouseEvent);

        void on_horizontalSlider_valueChanged(int value);

        void on_animationList_itemClicked(QListWidgetItem *item);

private:
        bool draw;
        QColor color;
        QPixmap tile;
        int tileIndex;
        QPoint startingPoint;
        Ui::MainWindow *ui;
        TileController* tileController;
        int intDelay;
};

#endif // MAINWINDOW_H
