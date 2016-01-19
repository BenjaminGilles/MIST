
#include <QtGui>
#include <QWidget>

#include "mprlayout.h"

MPRLayout::MPRLayout(QWidget *parent, int margin, int spacing)
    : QLayout(parent)
{
    setMargin(margin);
    setSpacing(spacing);
}

MPRLayout::MPRLayout(int spacing)
{
    setSpacing(spacing);
}

MPRLayout::~MPRLayout()
{
    QLayoutItem *l;
    while ((l = takeAt(0)))
        delete l;
}

void MPRLayout::addItem(QLayoutItem *item)
{
    add(item, XY);
}

void MPRLayout::addWidget(QWidget *widget, Position position)
{
    add(new QWidgetItem(widget), position);
}

Qt::Orientations MPRLayout::expandingDirections() const
{
    return Qt::Horizontal | Qt::Vertical;
}

bool MPRLayout::hasHeightForWidth() const
{
    return false;
}

int MPRLayout::count() const
{
    return list.size();
}

QLayoutItem *MPRLayout::itemAt(int index) const
{
    ItemWrapper *wrapper = list.value(index);
    if (wrapper)
        return wrapper->item;
    else
        return 0;
}

QSize MPRLayout::minimumSize() const
{
    return calculateSize(MinimumSize);
}

void MPRLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);

    QSize totalsize = calculateSize();
    QSize gridsize = gridSize();
    if(gridsize.width()==0 || gridsize.height()==0) return;

    double scalex = (rect.width()-(gridsize.width()-1)*spacing())/(double)totalsize.width(), scaley = (rect.height()-(gridsize.height()-1)*spacing())/(double)totalsize.height();
    if(scalex<scaley) scaley=scalex;    else scalex=scaley;
    QSize offset;

    for (int i = 0; i < list.size(); ++i)
    {
        ItemWrapper *wrapper = list.at(i);
        QLayoutItem *item = wrapper->item;
        if(item->widget()->isVisible())
        {
            if (wrapper->position == XY)
            {
                offset.rheight()=item->sizeHint().height()*scaley+spacing();
                offset.rwidth()=item->sizeHint().width()*scalex+spacing();
            }
            else if (wrapper->position == ZY) offset.rheight()=item->sizeHint().height()*scaley+spacing();
            else if (wrapper->position == XZ) offset.rwidth()=item->sizeHint().width()*scalex+spacing();
        }
    }

    for (int i = 0; i < list.size(); ++i)
    {
        ItemWrapper *wrapper = list.at(i);
        QLayoutItem *item = wrapper->item;
        if(item->widget()->isVisible())
        {
            if (wrapper->position == XY)             item->setGeometry(QRect(rect.x(), rect.y(), item->sizeHint().width()*scalex, item->sizeHint().height()*scaley));
            else if (wrapper->position == ZY)        item->setGeometry(QRect(rect.x()+offset.width(), rect.y(), item->sizeHint().width()*scalex, item->sizeHint().height()*scaley));
            else if (wrapper->position == XZ)        item->setGeometry(QRect(rect.x(), rect.y()+offset.height(), item->sizeHint().width()*scalex, item->sizeHint().height()*scaley));
        }
    }

}

QSize MPRLayout::sizeHint() const
{
    return calculateSize(SizeHint);
}

QLayoutItem *MPRLayout::takeAt(int index)
{
    if (index >= 0 && index < list.size()) {
        ItemWrapper *layoutStruct = list.takeAt(index);
        return layoutStruct->item;
    }
    return 0;
}

void MPRLayout::add(QLayoutItem *item, Position position)
{
    list.append(new ItemWrapper(item, position));
}

QSize MPRLayout::calculateSize(SizeType sizeType) const
{
    QSize totalSize;

    for (int i = 0; i < list.size(); ++i)
    {
        ItemWrapper *wrapper = list.at(i);
        Position position = wrapper->position;
        QSize itemSize = sizeType == MinimumSize ? wrapper->item->minimumSize() : wrapper->item->sizeHint();
        if(wrapper->item->widget()->isVisible())
        {
            if (position == XY || position == XZ)   totalSize.rwidth() = itemSize.width();
            if (position == XY || position == ZY)   totalSize.rheight() = itemSize.rheight();
        }
    }
    for (int i = 0; i < list.size(); ++i)
    {
        ItemWrapper *wrapper = list.at(i);
        Position position = wrapper->position;
        QSize itemSize = sizeType == MinimumSize ? wrapper->item->minimumSize() : wrapper->item->sizeHint();
        if(wrapper->item->widget()->isVisible())
        {
            if ( position == XZ)   totalSize.rheight() += itemSize.rheight();
            if ( position == ZY)   totalSize.rwidth() += itemSize.width();
        }
    }
    return totalSize;
}

QSize MPRLayout::gridSize() const
{
    QSize totalSize;
    totalSize.rwidth() = 1;
    totalSize.rheight() = 1;

    for (int i = 0; i < list.size(); ++i) {
        ItemWrapper *wrapper = list.at(i);
        if(wrapper->item->widget()->isVisible())
        {
            Position position = wrapper->position;
            if (position == XZ )           totalSize.rheight() += 1;
            if (position == ZY)            totalSize.rwidth() += 1;
        }
    }
    return totalSize;
}
