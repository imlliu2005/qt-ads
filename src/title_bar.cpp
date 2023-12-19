#include "title_bar.h"
#include "FloatingDockContainer.h"

#ifdef _DEBUG
#define new new (_NORMAL_BLOCK, __FILE__, __LINE__) // 定义new宏来替换new关键字。
#endif

    namespace ads
    {
        QTitleBar::QTitleBar(CFloatingDockContainer *parent) : QWidget(parent), FRAME_BUTTON_SIZE(30, 30)
        {
            this->m_frameButtons = QCustomAttrs::All;

            if (!parent)
            {
                throw std::invalid_argument("Parent must be a QCustomWindow object (cannot be null).");
            } 

            this->FloatingWidget = parent;
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
            connect(&this->btn_maximize, &QPushButton::clicked, this, &QTitleBar::_slot_maximize);

            connect(this, &QWidget::windowTitleChanged, &this->lbl_windowTitle, &QLabel::setText);
            this->setObjectName("QTitleBar");
        }

        void QTitleBar::set_titlebar_title(QString title) 
        {
            this->lbl_windowTitle.setText(title);
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
            QStyleOption opt;
            opt.init(this);
            QPainter p(this);
            style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
            QWidget::paintEvent(event);
        }

        void QTitleBar::mousePressEvent(QMouseEvent *event)
        {
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
            if (event->buttons() & Qt::LeftButton)
            {
                emit requestMaximize();
                event->accept();
            }
            else
            {
                QWidget::mouseDoubleClickEvent(event);
            }
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
            {
                // 还原事件
                set_maximize_icon();
            }
            else
            {
                // 最大化事件
                set_restore_icon();
            }
        }

    } // namespace ads

