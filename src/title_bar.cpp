#include "title_bar.h"

#ifdef _DEBUG
#define new new (_NORMAL_BLOCK, __FILE__, __LINE__) // 定义new宏来替换new关键字。
#endif

    namespace ads
    {

        QTitleBar::QTitleBar(QWidget *parent) : QWidget(parent), FRAME_BUTTON_SIZE(44, 34)
        {
            this->canMove = false;
            this->maximizing = false;
            this->m_frameButtons = QCustomAttrs::All;

//            this->setStyleSheet(QStringLiteral(
//                "QPushButton {\n"
//                "   border : 0px solid gray;\n"
//                "   border-radius: 0px;\n"
//                "   min-height: 20px;\n"
//                "   margin: 0px;\n"
//                "   background: transparent;\n"
//                "   color: gray;\n"
//                "}\n"
//                "QPushButton::hover {\n"
//                "   background: transparent;\n"
//                "   border : 0px solid darkorange;\n"
//                "   color: white;\n"
//                "}\n"
//                "QPushButton::pressed, QPushButton::!enabled {\n"
//                "   background: transparent;\n"
//                "   border : 0px solid orange;\n"
//                "   color: white;\n"
//                "}\n"
//                // "QTitleBar { background: rgb(58, 91, 164); }\n"
//                // "QWidget { background: rgb(58, 91, 164); }\n"
//                ));

            if (!parent)
                throw std::invalid_argument("Parent must be a QCustomWindow object (cannot be null).");
            this->m_parentWindow = parent;

            this->lbl_windowTitle.setText("QCustomWindow");
            this->lbl_windowTitle.setObjectName("TitleBar_title");
            this->lbl_windowTitle.setMouseTracking(true);
            setMouseTracking(true);
            // this->lbl_windowTitle.setAlignment(Qt::AlignCenter);
            this->lbl_windowTitle.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            // this->lbl_windowTitle.setStyleSheet("QLabel{background-color:rgb(58, 91, 164); color:rgb(255, 255, 255); }");

            this->btn_minimize.setIcon(QIcon(":/res/title_btn_minimize.png"));
            if (this->m_parentWindow->isMaximized())
                set_restore_icon();
            else
                set_maximize_icon();
            this->btn_close.setIcon(QIcon(":/res/title_btn_close.png"));
            this->btn_minimize.setObjectName("TitleBar_Btn_Minimize");
            this->btn_maximize.setObjectName("TitleBar_Btn_Maximize");
            this->btn_close.setObjectName("TitleBar_Btn_Close");
            // this->btn_minimize.setToolTip(QObject::tr("Minimize"));
            // this->btn_maximize.setToolTip(QObject::tr("Maximize"));
            // this->btn_close.setToolTip(QObject::tr("Close"));
            // this->btn_close.setText("X");
            // this->btn_maximize.setText("+");
            // this->btn_minimize.setText("-");

            this->btn_close.setMaximumSize(FRAME_BUTTON_SIZE);
            this->btn_close.setMinimumSize(FRAME_BUTTON_SIZE);
            this->btn_maximize.setMaximumSize(FRAME_BUTTON_SIZE);
            this->btn_maximize.setMinimumSize(FRAME_BUTTON_SIZE);
            this->btn_minimize.setMaximumSize(FRAME_BUTTON_SIZE);
            this->btn_minimize.setMinimumSize(FRAME_BUTTON_SIZE);
            this->m_layout.addStretch();
            this->m_layout.addWidget(&this->lbl_windowTitle, 1);
            this->m_layout.addWidget(&this->btn_minimize);
            this->m_layout.addWidget(&this->btn_maximize);
            this->m_layout.addWidget(&this->btn_close);
            this->m_layout.setContentsMargins(0, 0, 0, 0);
            this->m_layout.addStretch();

            this->setLayout(&this->m_layout);

            connect(&this->btn_close, &QPushButton::clicked, [this]
                    { emit this->requestClose(); });
            connect(&this->btn_minimize, &QPushButton::clicked, [this]
                    { emit this->requestMinimize(); });
            // connect(&this->btn_maximize, &QPushButton::clicked, [this]
            //         { emit this->requestMaximize(); });
            connect(&this->btn_maximize, &QPushButton::clicked, this, &QTitleBar::_slot_maximize);

            connect(this, &QWidget::windowTitleChanged, &this->lbl_windowTitle, &QLabel::setText);
            this->setObjectName("QTitleBar");

            // this->setMaximumHeight(34);
            // this->setMinimumHeight(34);
        }

        void QTitleBar::setWindowButtons(QCustomAttrs::WindowButtons btns)
        {
            this->m_frameButtons = btns;
            this->btn_close.setVisible(btns & QCustomAttrs::Close);
            this->btn_maximize.setVisible(btns & QCustomAttrs::Maximize);
            this->btn_minimize.setVisible(btns & QCustomAttrs::Minimize);
        }

        void QTitleBar::setWindowButtonEnabled(QCustomAttrs::WindowButton btn, bool enabled)
        {
            if (btn & QCustomAttrs::Close)
                this->btn_close.setEnabled(enabled);
            if (btn & QCustomAttrs::Maximize)
                this->btn_maximize.setEnabled(enabled);
            if (btn & QCustomAttrs::Minimize)
                this->btn_minimize.setEnabled(enabled);
        }

        void QTitleBar::paintEvent(QPaintEvent *event)
        {
            // LOG_EVERY_N(INFO, 100) << "titlebar paintEvent" << std::endl;
            QStyleOption opt;
            opt.init(this);
            QPainter p(this);
            style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
            QWidget::paintEvent(event);
        }

        void QTitleBar::mousePressEvent(QMouseEvent *event)
        {
            if (event->button() & Qt::LeftButton)
            {
                this->canMove = (event->x() > 5 && event->y() > 5 && event->x() < (this->m_parentWindow->width() - 5));
                this->m_pCursor = event->globalPos() - this->m_parentWindow->geometry().topLeft();
            }
            QWidget::mousePressEvent(event);
        }

        void QTitleBar::mouseMoveEvent(QMouseEvent *event)
        {
            if (!this->maximizing && canMove && event->buttons() & Qt::LeftButton && !this->m_parentWindow->isMaximized())
                this->m_parentWindow->move(event->globalPos() - m_pCursor);
            // else
                // LOG_WARN << "cannot move, isMaximized = " << this->m_parentWindow->isMaximized() << std::endl;
            this->maximizing = false;
            QWidget::mouseMoveEvent(event);
        }

        void QTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
        {
            if (m_frameButtons & QCustomAttrs::Maximize && btn_maximize.isEnabled() && event->buttons() & Qt::LeftButton)
            {
                // LOG_WARN << "mouseDoubleClickEvent request Maximize" << std::endl;
                this->maximizing = true;
                _maximize_restore_switch();
                emit requestMaximize();
            }
            // else
                // LOG_WARN << "mouseDoubleClickEvent not maximize" << std::endl;
            QWidget::mouseDoubleClickEvent(event);
        }

        void QTitleBar::_slot_maximize()
        {
            _maximize_restore_switch();
            emit this->requestMaximize();
        }

        void QTitleBar::set_restore_icon()
        {
            this->btn_maximize.setIcon(QIcon(":/res/title_btn_restore.png"));
        }

        void QTitleBar::set_maximize_icon()
        {
            this->btn_maximize.setIcon(QIcon(":/res/title_btn_maximize.png"));
        }

        void QTitleBar::_maximize_restore_switch() // 切换显示最大化|还原
        {
            if (this->m_parentWindow->isMaximized())
            // if(this->maximizing)
            {
                // 还原事件
                // LOG_WARN << "'''set to max icon" << std::endl;
                set_maximize_icon();
            }
            else
            {
                // 最大化事件
                // LOG_WARN << "'''set to restore icon" << std::endl;
                set_restore_icon();
            }
        }

    } // namespace ads

