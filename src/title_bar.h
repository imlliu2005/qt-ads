/*
 * @Description: 
 * @Author: binguanghong@aliyun.com
 * @LastEditors: Please set LastEditors
 * @Date: 2020-11-09 09:17:23
 * @LastEditTime: 2021-01-18 12:26:12
 * @Copyright: binguanghong@aliyun.com
 * @FilePath: \inc\medex\view\title_bar.h
 */
#ifndef medex_view_title_barH
#define medex_view_title_barH

#include <QtCore/QSize>
#include <QEvent>
#include <QLabel>
#include <QPoint>
#include <QWidget>
#include <QPainter>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPushButton>
#include <QStyleOption>
#include <stdexcept>
#include "ads_globals.h"

    namespace ads
    {
        class CFloatingDockContainer;

        namespace QCustomAttrs
        {
            enum WindowButton
            {
                Minimize = 0x01,
                Maximize = 0x02,
                Close = 0x04,
                All = Minimize | Maximize | Close
            };

            Q_DECLARE_FLAGS(WindowButtons, WindowButton)
            Q_DECLARE_OPERATORS_FOR_FLAGS(WindowButtons)
        } // namespace QCustomAttrs

        class QTitleBar : public QWidget
        {
            Q_OBJECT
            Q_PROPERTY(QCustomAttrs::WindowButtons windowButtons READ windowButtons WRITE setWindowButtons)
            Q_CLASSINFO("custom_obj_type", "QTitleBar")
        public:
            // explicit QTitleBar(QWidget *parent = nullptr);
            explicit QTitleBar(CFloatingDockContainer *parent = nullptr);

            void setWindowButtons(QCustomAttrs::WindowButtons btns);
            inline QCustomAttrs::WindowButtons windowButtons() const { return this->m_frameButtons; }

            void setWindowButtonEnabled(QCustomAttrs::WindowButton btn, bool enabled = true);
            void set_restore_icon();
            void set_maximize_icon();

        protected:
            void paintEvent(QPaintEvent *event) override;
            void mouseMoveEvent(QMouseEvent *event) override;
            void mouseReleaseEvent(QMouseEvent *ev) override;
            void mousePressEvent(QMouseEvent *event) override;
            void mouseDoubleClickEvent(QMouseEvent *event) override;

        private:
            void _maximize_restore_switch(); // 切换显示最大化|还原

            CFloatingDockContainer *FloatingWidget = nullptr;
            eDragState DragState = DraggingInactive;
            bool canMove;
            bool maximizing;

            QPoint m_pCursor;
            const QSize FRAME_BUTTON_SIZE;

            // QWidget *m_parentWindow;

            QCustomAttrs::WindowButtons m_frameButtons;

            QLabel lbl_windowTitle;
            QHBoxLayout m_layout;
            QPushButton btn_minimize;
            QPushButton btn_maximize;
            QPushButton btn_close;

        signals:
            void requestClose();
            void requestMaximize();
            void requestMinimize();
        private slots:
            void _slot_maximize();
        };

} // namespace ads

#endif
