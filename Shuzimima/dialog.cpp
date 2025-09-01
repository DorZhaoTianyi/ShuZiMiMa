#include "dialog.h"
#include <QTimer>
#include <algorithm>

Dialog::Dialog(QWidget *parent) : QDialog(parent)
{
    // 初始化UI设置
    setFixedSize(1060, 500);
    setWindowFlags(Qt::FramelessWindowHint);
    setStyleSheet("background-color:rgba(0, 0, 139, 50)");
    setAttribute(Qt::WA_DeleteOnClose);
    setMouseTracking(true);

    // 初始化成员变量
    w = width();
    h = height();
    m_radius = w > h ? h / 16 : w / 14;
    x_offset = w / 6;
    y_offset = -h / 2 + 20;
    m_currentMode = MODE_NONE;
    m_TryNum = 0;

    // 初始化串口
    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &Dialog::data_Receive);

    // 创建UI控件
    setupUI();
}

Dialog::~Dialog()
{
    if(serial && serial->isOpen()) {
        serial->close();
    }
    delete serial;
}

void Dialog::setupUI()
{
    // 关闭按钮
    m_closeButton = new QPushButton("×", this);
    m_closeButton->setStyleSheet("QPushButton { color: white; background: transparent; border: none; font-size: 24px; font-weight: bold; }"
                               "QPushButton:hover { color: #ff5555; }");
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->move(w - 35, 5);
    connect(m_closeButton, &QPushButton::clicked, this, &Dialog::close);

    // 串口配置控件
    setupSerialPortUI();

    // 创建圆形按钮网格
    setupCircleButtons();

    // 功能按钮
    setupFunctionButtons();

    // 状态标签
    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: white; font-size: 25px; background: transparent;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setFixedSize(w - 40, 30);
    m_statusLabel->move((w - m_statusLabel->width()) / 2 + 240, (h - m_statusLabel->height()) / 2 - 185);
    m_statusLabel->setText("请选择操作");
}

void Dialog::setupSerialPortUI()
{
    QFont labelFont;
    labelFont.setPointSize(20);

    QFont comboFont;
    comboFont.setPointSize(20);

    // 串口名称
    QLabel *nameLabel = new QLabel("串口名称:", this);
    nameLabel->setStyleSheet("color: white;");
    nameLabel->setFont(labelFont);
    nameLabel->move(20, 60);
    m_nameCombo = new QComboBox(this);
    m_nameCombo->setFixedSize(180, 50);
    m_nameCombo->setFont(comboFont);
    m_nameCombo->setStyleSheet("color:white");
    m_nameCombo->move(200, 50);

    // 波特率
    QLabel *botelvLabel = new QLabel("波特率:", this);
    botelvLabel->setStyleSheet("color: white;");
    botelvLabel->setFont(labelFont);
    botelvLabel->move(20, 130);
    m_botelvCombo = new QComboBox(this);
    m_botelvCombo->setFixedSize(180, 50);
    m_botelvCombo->setFont(comboFont);
    m_botelvCombo->setStyleSheet("color:white");
    m_botelvCombo->move(200, 120);
    m_botelvCombo->addItems({"9600", "19200", "38400", "57600", "115200"});

    // 校验位
    QLabel *jiaoyanweiLabel = new QLabel("校验位:", this);
    jiaoyanweiLabel->setStyleSheet("color: white;");
    jiaoyanweiLabel->setFont(labelFont);
    jiaoyanweiLabel->move(20, 200);
    m_jiaoyanweiCombo = new QComboBox(this);
    m_jiaoyanweiCombo->setFixedSize(180, 50);
    m_jiaoyanweiCombo->setFont(comboFont);
    m_jiaoyanweiCombo->setStyleSheet("color:white");
    m_jiaoyanweiCombo->move(200, 190);
    m_jiaoyanweiCombo->addItems({"None", "Odd", "Even"});

    // 数据位
    QLabel *shujuweiLabel = new QLabel("数据位:", this);
    shujuweiLabel->setStyleSheet("color: white;");
    shujuweiLabel->setFont(labelFont);
    shujuweiLabel->move(20, 270);
    m_shujuweiCombo = new QComboBox(this);
    m_shujuweiCombo->setFixedSize(180, 50);
    m_shujuweiCombo->setFont(comboFont);
    m_shujuweiCombo->setStyleSheet("color:white");
    m_shujuweiCombo->move(200, 260);
    m_shujuweiCombo->addItems({"5", "6", "7", "8"});

    // 停止位
    QLabel *tingzhiweiLabel = new QLabel("停止位:", this);
    tingzhiweiLabel->setStyleSheet("color: white;");
    tingzhiweiLabel->setFont(labelFont);
    tingzhiweiLabel->move(20, 340);
    m_tingzhiweiCombo = new QComboBox(this);
    m_tingzhiweiCombo->setFixedSize(180, 50);
    m_tingzhiweiCombo->setFont(comboFont);
    m_tingzhiweiCombo->setStyleSheet("color:white");
    m_tingzhiweiCombo->move(200, 330);
    m_tingzhiweiCombo->addItems({"1", "1.5", "2"});

    // 扫描和打开按钮
    QPushButton *scanBtn = new QPushButton("扫描串口", this);
    QPushButton *openBtn = new QPushButton("打开串口", this);

    QString btnStyle = "QPushButton {"
                      "   color: white; background: rgba(70, 130, 180, 150);"
                      "   border: 1px solid rgb(173, 216, 230); border-radius: 5px;"
                      "   padding: 5px; min-width: 80px;"
                      "   font-size: 15px;"
                      "}"
                      "QPushButton:hover { background: rgba(100, 150, 200, 150); }";

    scanBtn->setStyleSheet(btnStyle);
    openBtn->setStyleSheet(btnStyle);
    scanBtn->setFixedSize(120, 40);
    openBtn->setFixedSize(120, 40);
    int btnY = h - 80;
    int btnX = 50;

    scanBtn->move(btnX, btnY);
    openBtn->move(btnX + 180, btnY);

    connect(scanBtn, &QPushButton::clicked, this, &Dialog::on_scan_clicked);
    connect(openBtn, &QPushButton::clicked, this, &Dialog::on_open_clicked);
}

void Dialog::setupFunctionButtons()
{
    // 功能按钮
    m_setPasswordBtn = new QPushButton("设置密码", this);
    m_inputPasswordBtn = new QPushButton("输入密码", this);
    m_confirmBtn = new QPushButton("确认", this);

    QString btnStyle = "QPushButton {"
                      "   color: white; background: rgba(70, 130, 180, 150);"
                      "   border: 1px solid rgb(173, 216, 230); border-radius: 5px;"
                      "   padding: 5px; min-width: 80px;"
                      "}"
                      "QPushButton:hover { background: rgba(100, 150, 200, 150); }";

    m_setPasswordBtn->setStyleSheet(btnStyle);
    m_inputPasswordBtn->setStyleSheet(btnStyle);
    m_confirmBtn->setStyleSheet(btnStyle);

    m_setPasswordBtn->setFixedSize(120, 40);
    m_inputPasswordBtn->setFixedSize(120, 40);
    m_confirmBtn->setFixedSize(120, 40);

    int btnY = h - 60;
    int btnRightOffset = w / 3;

    m_setPasswordBtn->move(btnRightOffset + 230, btnY);
    m_inputPasswordBtn->move(btnRightOffset + 355, btnY);
    m_confirmBtn->move(btnRightOffset + 480, btnY);
    m_confirmBtn->setVisible(false);

    connect(m_setPasswordBtn, &QPushButton::clicked, this, &Dialog::onSetPasswordClicked);
    connect(m_inputPasswordBtn, &QPushButton::clicked, this, &Dialog::onInputPasswordClicked);
    connect(m_confirmBtn, &QPushButton::clicked, this, &Dialog::onConfirmClicked);
}

// 串口相关函数
void Dialog::on_scan_clicked()
{
    m_nameCombo->clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort com;
        com.setPort(info);
        if(com.open(QIODevice::ReadWrite))
        {
            m_nameCombo->addItem(info.portName());
            com.close();
        }
    }

    if(m_nameCombo->count() > 0) {
        m_statusLabel->setText("扫描完成，找到 " + QString::number(m_nameCombo->count()) + " 个串口");
    } else {
        m_statusLabel->setText("未找到可用串口");
    }
}

void Dialog::on_open_clicked()
{
    if(!serial->isOpen())
    {
        serialSet();
        if(serial->open(QIODevice::ReadWrite)) {
            m_statusLabel->setText("串口已打开");
            buffer.clear();
        } else {
            m_statusLabel->setText("无法打开串口");
        }
    }
    else
    {
        serial->close();
        m_statusLabel->setText("串口已关闭");
    }
}

void Dialog::serialSet()
{
    serial->setPortName(m_nameCombo->currentText());
    serial->setBaudRate(m_botelvCombo->currentText().toInt());

    switch (m_tingzhiweiCombo->currentIndex())
    {
        case 0: serial->setStopBits(QSerialPort::OneStop); break;
        case 1: serial->setStopBits(QSerialPort::OneAndHalfStop); break;
        case 2: serial->setStopBits(QSerialPort::TwoStop); break;
    }

    switch (m_shujuweiCombo->currentText().toInt())
    {
        case 5: serial->setDataBits(QSerialPort::Data5); break;
        case 6: serial->setDataBits(QSerialPort::Data6); break;
        case 7: serial->setDataBits(QSerialPort::Data7); break;
        case 8: serial->setDataBits(QSerialPort::Data8); break;
    }

    switch (m_jiaoyanweiCombo->currentIndex())
    {
        case 0: serial->setParity(QSerialPort::NoParity); break;
        case 1: serial->setParity(QSerialPort::OddParity); break;
        case 2: serial->setParity(QSerialPort::EvenParity); break;
    }
}

// 数据接收和处理
void Dialog::data_Receive()
{
    buffer.append(serial->readAll());

    while(buffer.size() >= 26)
    {
        if(static_cast<uint8_t>(buffer[0]) == 0x0A &&
           static_cast<uint8_t>(buffer[25]) == 0x0B)
        {
            // 解析压力值
            P11 = static_cast<uint16_t>(buffer[1] << 8) | static_cast<uint8_t>(buffer[2]);
            P12 = static_cast<uint16_t>(buffer[3] << 8) | static_cast<uint8_t>(buffer[4]);
            P13 = static_cast<uint16_t>(buffer[5] << 8) | static_cast<uint8_t>(buffer[6]);
            P14 = static_cast<uint16_t>(buffer[7] << 8) | static_cast<uint8_t>(buffer[8]);  // 新增第一行第四列

            P21 = static_cast<uint16_t>(buffer[9] << 8) | static_cast<uint8_t>(buffer[10]);
            P22 = static_cast<uint16_t>(buffer[11] << 8) | static_cast<uint8_t>(buffer[12]);
            P23 = static_cast<uint16_t>(buffer[13] << 8) | static_cast<uint8_t>(buffer[14]);
            P24 = static_cast<uint16_t>(buffer[15] << 8) | static_cast<uint8_t>(buffer[16]);  // 新增第二行第四列

            P31 = static_cast<uint16_t>(buffer[17] << 8) | static_cast<uint8_t>(buffer[18]);
            P32 = static_cast<uint16_t>(buffer[19] << 8) | static_cast<uint8_t>(buffer[20]);
            P33 = static_cast<uint16_t>(buffer[21] << 8) | static_cast<uint8_t>(buffer[22]);
            P34 = static_cast<uint16_t>(buffer[23] << 8) | static_cast<uint8_t>(buffer[24]);  // 新增第三行第四列

            // 检查每个压力值
            checkPressureValue(0, 0, P11);  // 第一行第一列
            checkPressureValue(0, 1, P12);  // 第一行第二列
            checkPressureValue(0, 2, P13);  // 第一行第三列
            checkPressureValue(0, 3, P14);  // 新增第一行第四列

            checkPressureValue(1, 0, P21);  // 第二行第一列
            checkPressureValue(1, 1, P22);  // 第二行第二列
            checkPressureValue(1, 2, P23);  // 第二行第三列
            checkPressureValue(1, 3, P24);  // 新增第二行第四列

            checkPressureValue(2, 0, P31);  // 第三行第一列
            checkPressureValue(2, 1, P32);  // 第三行第二列
            checkPressureValue(2, 2, P33);  // 第三行第三列
            checkPressureValue(2, 3, P34);  // 新增第三行第四列

            buffer.remove(0, 25);  // 增加缓冲区移除的大小
        }
        else
        {
            buffer.remove(0, 1);
        }
    }
}

void Dialog::checkPressureValue(int row, int col, int value)
{
    if (value > 4000) {
        // 找到对应的按钮
        for (QPushButton* btn : m_circleButtons) {
            if (btn->property("gridX").toInt() == row &&
                btn->property("gridY").toInt() == col) {
                // 模拟按钮点击
                onCircleButtonClicked(btn);
                break;
            }
        }
    }
}

// 圆形按钮点击处理
void Dialog::setupCircleButtons()
{
    // 创建圆形按钮 (3x4网格)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            QPushButton* btn = new QPushButton(this);
            btn->setFixedSize(m_radius * 2, m_radius * 2);
            btn->setStyleSheet(
                "QPushButton {"
                "   border: 2px solid rgb(173, 216, 230);"
                "   border-radius: " + QString::number(m_radius) + "px;"
                "   background: transparent;"
                "}"
                "QPushButton:hover {"
                "   background: rgba(173, 216, 230, 50);"
                "}"
                "QPushButton:pressed {"
                "   background: rgba(173, 216, 230, 100);"
                "   font-size: 15px;"
                "}");

            // 计算按钮位置 - 调整y坐标向上偏移50像素
            int x = w/2 + x_offset + (i * 4 - 2) * m_radius - m_radius;
            int y = h/2 + y_offset + (4 + j * 3) * m_radius - m_radius - 30;  // 原值+20改为-30，整体上移50像素
            btn->move(x, y);

            // 存储按钮信息
            btn->setProperty("gridX", i);
            btn->setProperty("gridY", j);

            // 使用lambda表达式连接信号槽
            connect(btn, &QPushButton::clicked, this, [this, btn]() {
                this->onCircleButtonClicked(btn);
            });

            m_circleButtons.append(btn);
        }
    }
}

void Dialog::onCircleButtonClicked(QPushButton* btn)
{
    int gridX = btn->property("gridX").toInt();
    int gridY = btn->property("gridY").toInt();

    // Define the text content for each circle
    QString circleTexts[4][3] = {
        {"1", "2", "3"},
        {"4", "5", "6"},
        {"7", "8", "9"},
        {"*", "0", "#"}
    };

    QString clickedText = circleTexts[gridY][gridX];

    if (m_currentMode == MODE_NONE) {
        // In normal mode, just show the clicked content
        m_statusLabel->setText("你点击了: " + clickedText);
        return;
    }

    int order = gridY * 3 + gridX;
    // 移除检查是否已存在的条件，允许重复点击
    m_TouchPoints.push_back(QPoint(gridX, gridY));
    m_TouchOrder.push_back(order);

    // 不再修改按钮样式，保持原样
    // btn->setStyleSheet(...);

    // Update status label based on current mode
    if (m_currentMode == MODE_SET) {
        m_statusLabel->setText("请设置密码: " + getCurrentSequenceText());
    } else if (m_currentMode == MODE_INPUT) {
        m_statusLabel->setText("请输入密码: " + getCurrentSequenceText());
    }

    if (m_TouchPoints.size() >= 2) {
        m_confirmBtn->setVisible(true);
    }
    update();
}

QString Dialog::getCurrentSequenceText()
{
    QString sequence;
    QString circleTexts[4][3] = {
        {"1", "2", "3"},
        {"4", "5", "6"},
        {"7", "8", "9"},
        {"*", "0", "#"}
    };

    for (int i = 0; i < m_TouchOrder.size(); ++i) {
        int order = m_TouchOrder[i];
        int gridY = order / 3;
        int gridX = order % 3;
        sequence += circleTexts[gridY][gridX];
//        if (i < m_TouchOrder.size() - 1) {
//            sequence += ", ";
//        }
    }
    return sequence;
}

// 其他功能函数
void Dialog::onSetPasswordClicked()
{
    m_currentMode = MODE_SET;
    m_TouchPoints.clear();
    m_TouchOrder.clear();
    m_confirmBtn->setVisible(false);
    m_statusLabel->setText("请设置密码");

    // Reset all circle buttons style
    resetCircleButtons();
    update();
}

void Dialog::onInputPasswordClicked()
{
    if (m_setPassword.empty()) {
        m_statusLabel->setText("请先设置密码");
        m_currentMode = MODE_SET;
    } else {
        m_currentMode = MODE_INPUT;
        m_TryNum = 0;
        m_TouchPoints.clear();
        m_TouchOrder.clear();
        m_confirmBtn->setVisible(false);
        m_statusLabel->setText("请输入密码");

        // Reset all circle buttons style
        resetCircleButtons();
    }
    update();
}

void Dialog::onConfirmClicked()
{
    switch(m_currentMode) {
    case MODE_SET:
        if (m_TouchOrder.size() >= 2) {
            m_setPassword = m_TouchOrder;
            m_currentMode = MODE_NONE;
            m_statusLabel->setText("密码设置成功!");
            m_TouchPoints.clear();
            m_TouchOrder.clear();
            m_confirmBtn->setVisible(false);
        } else {
            m_statusLabel->setText("密码至少需要连接2个点");
        }
        break;

    case MODE_INPUT:
        if (!m_TouchOrder.empty()) {
            if (m_TouchOrder == m_setPassword) {
                m_statusLabel->setText("验证通过!");
                QTimer::singleShot(1000, this, &Dialog::close);
            } else {
                m_TryNum++;
                if (m_TryNum >= 3) {
                    m_statusLabel->setText("尝试次数过多，请重新设置密码");
                    m_TryNum = 0;
                    m_setPassword.clear();
                    m_currentMode = MODE_SET;
                } else {
                    m_statusLabel->setText(QString("密码错误，还有%1次机会").arg(3 - m_TryNum));
                }
                m_TouchPoints.clear();
                m_TouchOrder.clear();
                m_confirmBtn->setVisible(false);
                resetCircleButtons();
            }
        }
        break;
    }
    update();
}

void Dialog::resetCircleButtons()
{
    for (QPushButton* btn : m_circleButtons) {
        btn->setStyleSheet(
            "QPushButton {"
            "   border: 2px solid rgb(173, 216, 230);"
            "   border-radius: " + QString::number(m_radius) + "px;"
            "   background: transparent;"
            "}"
            "QPushButton:hover {"
            "   background: rgba(173, 216, 230, 50);"
            "}"
            "QPushButton:pressed {"
            "   background: rgba(173, 216, 230, 100);"
            "}");
    }
}

void Dialog::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::white);

    QString hintText;
    switch(m_currentMode) {
    case MODE_SET: hintText = "请设置新手势密码"; break;
    case MODE_INPUT: hintText = "请输入手势密码"; break;
    default: hintText = "请选择操作";
    }

    QFont font;
    font.setPixelSize(24);
    font.setBold(true);
    painter->setFont(font);

    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(hintText);
    int textHeight = fm.height();

    int x = (w - textWidth) / 2 - 550;
    int y = (h - textHeight) / 2 - 450;

    painter->drawText(x, y, hintText);
    painter->restore();
}

void Dialog::drawJoinLine(QPainter *painter)
{
    if (m_TouchPoints.size() < 2) return;

    painter->save();
    QPen pen(QColor(173, 216, 230), 16);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    for (int i = 0; i < m_TouchPoints.size() - 1; i++) {
        QPoint p1 = m_TouchPoints[i];
        QPoint p2 = m_TouchPoints[i+1];

        QPointF center1(x_offset + (p1.x() * 4 - 1) * m_radius - 30,
                       y_offset + (3 + p1.y() * 4) * m_radius + 20);
        QPointF center2(x_offset + (p2.x() * 4 - 1) * m_radius - 30,
                       y_offset + (3 + p2.y() * 4) * m_radius + 20);

        painter->drawLine(center1, center2);
    }
    painter->restore();
}

QPoint Dialog::getCircleCenter(int gridX, int gridY) const
{
    int x = w/2 + x_offset + (gridX * 4 - 2) * m_radius;
    int y = h/2 + y_offset + (4 + gridY * 3) * m_radius - 30;  // 同样调整y坐标
    return QPoint(x, y);
}

void Dialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制标题"数字密码验证系统"
    painter.setPen(Qt::white);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    QRect titleRect(0, 20, width(), 30);  // 顶部居中位置
    painter.drawText(titleRect, Qt::AlignCenter, "数字密码验证系统");

    // 定义圆圈内的文本内容
    QString circleTexts[4][3] = {
        {"1", "2", "3"},
        {"4", "5", "6"},
        {"7", "8", "9"},
        {"*", "0", "#"}
    };

    // 绘制所有圆圈和文本
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 3; ++col) {
            QPoint center = getCircleCenter(col, row);

            // 绘制圆圈 - 修改为浅蓝色填充
            painter.setPen(QColor(100, 149, 237));  // 浅蓝色边框
            painter.setBrush(QColor(100, 149, 237));  // 浅蓝色填充
            painter.drawEllipse(center, m_radius, m_radius);

            // 绘制文本 - 修改为白色加粗
            painter.setPen(Qt::white);  // 白色文字
            QFont font = painter.font();
            font.setPointSize(20);
            font.setBold(true);  // 加粗
            painter.setFont(font);

            QRect textRect(center.x() - m_radius, center.y() - m_radius,
                          m_radius * 2, m_radius * 2);
            painter.drawText(textRect, Qt::AlignCenter, circleTexts[row][col]);
        }
    }
}

void Dialog::handleCircleButtonClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        onCircleButtonClicked(btn);
    }
}
