#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QMap>
#include <QPair>
#include <QVector>
#include <QPoint>

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    enum Mode {
        MODE_NONE,
        MODE_SET,
        MODE_INPUT
    };

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_scan_clicked();
    void on_open_clicked();
    void data_Receive();
    void onCircleButtonClicked(QPushButton* btn);
    void handleCircleButtonClicked();
    void onBackClicked();
    void onResetClicked();
    void onSetPasswordClicked();
    void onInputPasswordClicked();
    void onConfirmClicked();

private:
    void setupUI();
    void setupSerialPortUI();
    void setupCircleButtons();
    void setupFunctionButtons();
    void setupControlButtons();
    void serialSet();
    void checkPressureValue(int row, int col, int value);
    void resetCircleButtons();
    QString getCurrentSequenceText();
    void drawBg(QPainter *painter);
    void drawJoinLine(QPainter *painter);
    QPoint getCircleCenter(int gridX, int gridY) const;

    // UI控件
    QPushButton *m_closeButton;
    QComboBox *m_nameCombo;
    QComboBox *m_botelvCombo;
    QComboBox *m_jiaoyanweiCombo;
    QComboBox *m_shujuweiCombo;
    QComboBox *m_tingzhiweiCombo;
    QLabel *m_statusLabel;
    QPushButton *m_setPasswordBtn;
    QPushButton *m_inputPasswordBtn;
    QPushButton *m_confirmBtn;
    QPushButton *m_backButton;
    QPushButton *m_resetButton;

    // 串口
    QSerialPort *serial;
    QByteArray buffer;

    // 压力值变量
    uint16_t P11, P12, P13, P14;
    uint16_t P21, P22, P23, P24;
    uint16_t P31, P32, P33, P34;

    // 应用程序变量
    int w, h;
    int m_radius;
    int x_offset;
    int y_offset;
    Mode m_currentMode;
    int m_TryNum;
    bool m_pressureState[3][4];
    QVector<QPushButton*> m_circleButtons;
    QVector<QPoint> m_TouchPoints;
    QVector<int> m_TouchOrder;
    QVector<int> m_setPassword;
    QPair<int, int> m_lastPressedButton;
};

#endif // DIALOG_H
