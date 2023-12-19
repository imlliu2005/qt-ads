#include "title_bar.h"
#include "FloatingDockContainer.h"

#ifdef _DEBUG
#define new new (_NORMAL_BLOCK, __FILE__, __LINE__) // 定义new宏来替换new关键字。
#endif

    namespace ads
    {

        QTitleBar::QTitleBar(CFloatingDockContainer *parent) : QWidget(parent), FRAME_BUTTON_SIZE(44, 34)
        {
            this->canMove = false;
            this->maximizing = false;
            this->m_frameButtons = QCustomAttrs::All;

            if (!parent)
            {
                throw std::invalid_argument("Parent must be a QCustomWindow object (cannot be null).");
            } 
            this->FloatingWidget = parent;

            this->lbl_windowTitle.setText("QCustomWindow");
            this->lbl_windowTitle.setObjectName("TitleBar_title");
            this->lbl_windowTitle.setMouseTracking(true);
            setMouseTracking(true);
            this->lbl_windowTitle.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            this->btn_minimize.setIcon(QIcon(":/res/title_btn_minimize.png"));
            if (this->FloatingWidget->isMaximized())
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
            // if (event->button() & Qt::LeftButton)
            // {
            //     this->canMove = (event->x() > 5 && event->y() > 5 && event->x() < (this->FloatingWidget->width() - 5));
            //     this->m_pCursor = event->globalPos() - this->FloatingWidget->geometry().topLeft();
            // }

            if (event->button() == Qt::LeftButton)
            {
                DragState = DraggingFloatingWidget;
                FloatingWidget->startDragging(event->pos(), FloatingWidget->size(),
                    this);
                return;
            }
            QWidget::mousePressEvent(event);
        }
        //============================================================================
        void QTitleBar::mouseReleaseEvent(QMouseEvent *event)
        {
            DragState = DraggingInactive;
            if (FloatingWidget)
            {
                FloatingWidget->finishDragging();
            }
            QWidget::mouseReleaseEvent(event);
        }

        void QTitleBar::mouseMoveEvent(QMouseEvent *event)
        {
            // if (!this->maximizing && canMove && event->buttons() & Qt::LeftButton && !this->FloatingWidget->isMaximized())
            // {
            //      this->FloatingWidget->move(event->globalPos() - m_pCursor);
            // }
      
            // this->maximizing = false;
            if (!(event->buttons() & Qt::LeftButton) || DraggingInactive == DragState)
            {
                DragState = DraggingInactive;
                QWidget::mouseMoveEvent(event);
                return;
            }

            // move floating window
            if (DraggingFloatingWidget == DragState)
            {
                if(FloatingWidget->isMaximized())
                {
                    FloatingWidget->showNormal();
                }
                FloatingWidget->moveFloating();
                QWidget::mouseMoveEvent(event);
                return;
            }
            QWidget::mouseMoveEvent(event);
        }

        void QTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
        {
            if (m_frameButtons & QCustomAttrs::Maximize && btn_maximize.isEnabled() && event->buttons() & Qt::LeftButton)
            {
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
            if (this->FloatingWidget->isMaximized())
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

