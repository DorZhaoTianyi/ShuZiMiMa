#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <vector>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QList>

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
    void onSetPasswordClicked();
    void onInputPasswordClicked();
    void onConfirmClicked();
    void on_scan_clicked();
    void on_open_clicked();
    void data_Receive();
    void handleCircleButtonClicked();

private:
    // UI初始化方法
    void setupUI();
    void setupSerialPortUI();
    void setupCircleButtons();
    void setupFunctionButtons();
    void resetCircleButtons();


    // 绘图方法
    void drawBg(QPainter *painter);
    void drawJoinLine(QPainter *painter);
    void drawCircleText(QPainter *painter);  // 新增：绘制圆圈文本
    QPoint getCircleCenter(int gridX, int gridY) const;
    QString getCurrentSequenceText();

    // 功能方法
    void serialSet();
    void checkPressureValue(int row, int col, int value);
    void onCircleButtonClicked(QPushButton* btn);

    // 串口相关
    QSerialPort *serial;
    QByteArray buffer;
    QVector<QVector<int>> allData;

    // 压力传感器值
    int P11, P12, P13, P14;
    int P21, P22, P23, P24;
    int P31, P32, P33, P34;

    // UI控件
    QPushButton *m_closeButton;
    QPushButton *m_setPasswordBtn;
    QPushButton *m_inputPasswordBtn;
    QPushButton *m_confirmBtn;
    QList<QPushButton*> m_circleButtons;
    QLabel *m_statusLabel;

    // 串口配置控件
    QComboBox *m_nameCombo;
    QComboBox *m_botelvCombo;
    QComboBox *m_jiaoyanweiCombo;
    QComboBox *m_shujuweiCombo;
    QComboBox *m_tingzhiweiCombo;

    // 几何参数
    int w;
    int h;
    int m_radius;
    int x_offset;
    int y_offset;

    // 圆圈文本内容
    QVector<QVector<QString>> m_circleTexts;  // 新增：存储圆圈文本

    // 状态变量
    int m_TryNum;
    Mode m_currentMode;

    std::vector<QPoint> m_TouchPoints;
    std::vector<int> m_TouchOrder;
    std::vector<int> m_setPassword;
};

#endif // DIALOG_H
