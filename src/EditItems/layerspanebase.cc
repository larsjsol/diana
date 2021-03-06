/*
  Diana - A Free Meteorological Visualisation Tool

  $Id$

  Copyright (C) 2013 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
  NORWAY
  email: diana@met.no

  This file is part of Diana

  Diana is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Diana is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Diana; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <EditItems/layerspanebase.h>
#include <EditItems/layergroup.h>
#include <EditItems/layermanager.h>
#include <EditItems/kml.h>
#include <EditItems/dialogcommon.h>
#include <EditItems/modifylayerscommand.h>
#include <diEditItemManager.h>

#include <QAction>
#include <QApplication>
#include <QBitArray>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPixmap>
#include <QString>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "edit.xpm"
#include "movedown.xpm"
#include "moveup.xpm"
#include "visible.xpm"
#include "unsavedchanges.xpm"

namespace EditItems {

LayerWidget::LayerWidget(
    LayerManager *layerManager, const QSharedPointer<Layer> &layer, bool showInfo, bool addVisibleLabel, QWidget *parent)
  : QWidget(parent)
  , layerMgr_(layerManager)
  , layer_(layer)
  , visibleLabel_(0)
{
  setContentsMargins(0, 0, 0, 0);

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setMargin(0);
  mainLayout->setSpacing(0);

  if (addVisibleLabel) {
    visibleLabel_ = new CheckableLabel(
          layer_ ? layer_->isVisible() : false, QPixmap(visible_xpm), "the layer is visible\n(click to make it invisible)", "the layer is invisible\n(click to make it visible)");
    if (layer_)
      connect(layer_.data(), SIGNAL(visibilityChanged(bool)), visibleLabel_, SLOT(checkAndNotify(bool)), Qt::UniqueConnection);
    connect(visibleLabel_, SIGNAL(checked(bool)), SLOT(handleVisibilityChanged(bool)));
    mainLayout->addWidget(visibleLabel_);
  }

  static int nn = 0;
  unsavedChangesLabel_ = new CheckableLabel(
    nn++ % 2, QPixmap(unsavedchanges_xpm), "the layer has unsaved changes\n(do ??? to save them)", "the layer does not have any unsaved changes", false);
  connect(unsavedChangesLabel_, SIGNAL(mouseClicked(QMouseEvent *)), SIGNAL(mouseClicked(QMouseEvent *)));
  //mainLayout->addWidget(unsavedChangesLabel_);

  nameLabel_ = new ClickableLabel(layer_ ? layer_->name() : QString());
  nameLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  connect(nameLabel_, SIGNAL(mouseClicked(QMouseEvent *)), SIGNAL(mouseClicked(QMouseEvent *)));
  connect(nameLabel_, SIGNAL(mouseDoubleClicked(QMouseEvent *)), SIGNAL(mouseDoubleClicked(QMouseEvent *)));
  mainLayout->addWidget(nameLabel_);

  infoLabel_ = new ClickableLabel("<info>");
  infoLabel_->setVisible(showInfo);
  infoLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  connect(infoLabel_, SIGNAL(mouseClicked(QMouseEvent *)), SIGNAL(mouseClicked(QMouseEvent *)));
  connect(infoLabel_, SIGNAL(mouseDoubleClicked(QMouseEvent *)), SIGNAL(mouseDoubleClicked(QMouseEvent *)));
  mainLayout->addWidget(infoLabel_);

  mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Preferred));
}

LayerWidget::~LayerWidget()
{
  if (visibleLabel_)
    delete visibleLabel_;
  delete unsavedChangesLabel_;
  delete nameLabel_;
  delete infoLabel_;
}

QSharedPointer<Layer> LayerWidget::layer() const
{
  return layer_;
}

QString LayerWidget::name() const
{
  return layer_->name();
}

void LayerWidget::setName(const QString &name)
{
  const QString trimmedName = name.trimmed();
  layer_->setName(trimmedName);
  updateLabels();
}

bool LayerWidget::isLayerVisible() const
{
  return layer_->isVisible();
}

void LayerWidget::setLayerVisible(bool visible)
{
  if (visibleLabel_)
    visibleLabel_->setChecked(visible);
  layer_->setVisible(visible);
}

bool LayerWidget::hasUnsavedChanges() const
{
  return layer_->hasUnsavedChanges();
}

void LayerWidget::setUnsavedChanges(bool unsavedChanges)
{
  unsavedChangesLabel_->setChecked(unsavedChanges);
}

void LayerWidget::setSelected(bool selected)
{
  const QString ssheet(selected ? "QLabel { background-color : #f27b4b; color : black; }" : "");
  if (visibleLabel_)
    visibleLabel_->setStyleSheet(ssheet);
  unsavedChangesLabel_->setStyleSheet(ssheet);
  nameLabel_->setStyleSheet(ssheet);
  infoLabel_->setStyleSheet(ssheet);
  layer_->setSelected(selected);
}

void LayerWidget::editName()
{
  QString name = layer_->name();

  while (true) {
    bool ok;
    name = QInputDialog::getText(this, "Edit layer name", "Layer name:", QLineEdit::Normal, name, &ok).trimmed();
    if (ok) {
      const QSharedPointer<Layer> existingLayer = layerMgr_->findLayer(name);
      if (!existingLayer) {
        setName(name);
        break; // ok (changed)
      } else if (existingLayer == layer()) {
        break; // ok (unchanged)
      } else {
        QMessageBox::warning(this, "Name exists", "Another layer with this name already exists!", QMessageBox::Ok);
        // try again
      }
    } else {
      break; // cancel
    }
  }
}

void LayerWidget::setState(const QSharedPointer<Layer> &layer)
{
  Q_ASSERT(layer);
  layer_ = layer;
  if (visibleLabel_) {
    visibleLabel_->setChecked(layer->isVisible());
    connect(layer_.data(), SIGNAL(visibilityChanged(bool)), visibleLabel_, SLOT(checkAndNotify(bool)), Qt::UniqueConnection);
  }
  unsavedChangesLabel_->setChecked(layer->hasUnsavedChanges());
  nameLabel_->setText(layer->name());
  setSelected(layer->isSelected());
}

void LayerWidget::showInfo(bool checked)
{
  infoLabel_->setVisible(checked);
  updateLabels();
}

bool LayerWidget::isRemovable() const
{
  return layer_.isNull() ? true : layer_->isRemovable();
}

bool LayerWidget::isAttrsEditable() const
{
  return layer_.isNull() ? false : layer_->isAttrsEditable();
}

void LayerWidget::updateLabels()
{
  const QString nameText = QString("%1").arg(layer_->name());
  nameLabel_->setText(nameText);

  const QString infoText =
      QString(" (editable=%1, nitems=%2, selected items=%3)")
      .arg(layer_->isEditable())
      .arg(layer_->itemCount())
      .arg(layer_->selectedItemCount());
  infoLabel_->setText(infoText);

//  const QString ssheet(layer_->layerGroupRef()->isActive() ? "QLabel { background-color : #f27b4b; color : black; }" : "");
//  nameLabel_->setStyleSheet(ssheet);
//  infoLabel_->setStyleSheet(ssheet);
}

void LayerWidget::handleVisibilityChanged(bool visible)
{
//  layer_->setVisible(visible); ### obsolete
  emit visibilityChanged(layer_, visible);
}

LayersPaneBase::LayersPaneBase(EditItems::LayerManager *layerManager, const QString &title, bool visibleLabelsEnabled, bool undoEnabled, bool multiSelectable)
  : showAllButton_(0)
  , hideAllButton_(0)
  , moveUpButton_(0)
  , moveDownButton_(0)
  , editButton_(0)
  , importFilesButton_(0)
  , showInfo_(false)
  , visibleLabelsEnabled_(visibleLabelsEnabled)
  , layerMgr_(layerManager)
  , undoEnabled_(undoEnabled)
  , visibleLayerWidget_(0)
  , multiSelectable_(multiSelectable)
  , layerUpdatesEnabled_(true)
{
  QVBoxLayout *vboxLayout1 = new QVBoxLayout;
  vboxLayout1->setContentsMargins(0, 2, 0, 2);

  QWidget *activeLayersWidget = new QWidget;
  activeLayersWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  layout_ = new QVBoxLayout(activeLayersWidget);
  layout_->setContentsMargins(0, 0, 0, 0);
  layout_->setSpacing(0);
  layout_->setMargin(0);

  scrollArea_ = new ScrollArea;
  scrollArea_->setWidget(activeLayersWidget);
  scrollArea_->setWidgetResizable(true);

  vboxLayout1->addWidget(scrollArea_);

  bottomLayout_ = new QHBoxLayout; // populated by subclass
  vboxLayout1->addLayout(bottomLayout_);

  QGroupBox *groupBox = new QGroupBox(title);
  groupBox->setLayout(vboxLayout1);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(groupBox);
  setLayout(mainLayout);

  connect(layerMgr_, SIGNAL(stateReplaced()), SLOT(update()));
}

void LayersPaneBase::init()
{
  update();
}

void LayersPaneBase::showInfo(bool checked)
{
  for (int i = 0; i < layout_->count(); ++i) {
    LayerWidget *layerWidget = qobject_cast<LayerWidget *>(layout_->itemAt(i)->widget());
    layerWidget->showInfo(checked);
  }
  showInfo_ = checked;
}

void LayersPaneBase::initLayerWidget(LayerWidget *layerWidget)
{
  connect(layerWidget, SIGNAL(mouseClicked(QMouseEvent *)), SLOT(mouseClicked(QMouseEvent *)), Qt::UniqueConnection);
  connect(layerWidget, SIGNAL(mouseDoubleClicked(QMouseEvent *)), SLOT(mouseDoubleClicked(QMouseEvent *)), Qt::UniqueConnection);
  connect(layerWidget, SIGNAL(visibilityChanged(const QSharedPointer<Layer> &, bool)),
          SLOT(handleVisibilityChanged(const QSharedPointer<Layer> &, bool)), Qt::UniqueConnection);
  if (layerWidget->layer()) {
    layerWidget->updateLabels();
    connect(layerWidget->layer().data(), SIGNAL(updated()), SLOT(handleLayersUpdate()), Qt::UniqueConnection);
    connect(layerWidget->layer().data(), SIGNAL(updated()), layerWidget, SLOT(updateLabels()), Qt::UniqueConnection);
  }
}

void LayersPaneBase::keyPressEvent(QKeyEvent *event)
{
  // try special handling
  if (handleKeyPressEvent(event))
    return;

  // try common handling
  if (((event->key() == Qt::Key_Up) || (event->key() == Qt::Key_Down)) && (layerMgr_->selectedLayers().size() == 1)) {

    const int selPos = selectedPos().first();

    if (event->key() == Qt::Key_Up) {
      if (event->modifiers() & Qt::ControlModifier) {
        moveSingleSelectedUp();
      } else if (selPos > 0) {
        // single-select the layer above the currently selected one
        if (undoEnabled_) {
          QBitArray selected(layerMgr_->orderedLayers().size());
          selected.setBit(layerMgr_->orderedLayers().indexOf(atPos(selPos - 1)->layer()));
          if (selected != layerMgr_->selected())
            EditItemManager::instance()->undoStack()->push(new ModifyLayerSelectionCommand(layerMgr_, selected));
        } else {
          selectIndex(selPos, false);
          selectIndex(selPos - 1);
        }
      }
    } else if (event->key() == Qt::Key_Down) {
      if (event->modifiers() & Qt::ControlModifier) {
        moveSingleSelectedDown();
      } else if (selPos < (layout_->count() - 1)) {
        // single-select the layer below the currently selected one
        if (undoEnabled_) {
          QBitArray selected(layerMgr_->orderedLayers().size());
          selected.setBit(layerMgr_->orderedLayers().indexOf(atPos(selPos + 1)->layer()));
          if (selected != layerMgr_->selected())
            EditItemManager::instance()->undoStack()->push(new ModifyLayerSelectionCommand(layerMgr_, selected));
        } else {
          selectIndex(selPos, false);
          selectIndex(selPos + 1);
        }
      }
    }
  } else {
    QWidget::keyPressEvent(event);
  }
}

void LayersPaneBase::selectIndex(int index, bool selected)
{
  select(atPos(index), selected);
}

void LayersPaneBase::select(const QList<LayerWidget *> &layerWidgets, bool selected)
{
  LayerWidget *visLW = 0;
  bool updateNeeded = false;
  foreach (LayerWidget *lw, layerWidgets) {
    if (lw) {
      lw->setSelected(selected);
      updateNeeded = true;
      if (selected && !visLW)
        visLW = lw;
    }
  }
  if (visLW)
    ensureVisible(visLW);
  if (updateNeeded)
    handleWidgetsUpdate();
}

void LayersPaneBase::select(LayerWidget *layerWidget, bool selected)
{
  select(QList<LayerWidget *>() << layerWidget, selected);
}

void LayersPaneBase::selectExclusive(const QList<LayerWidget *> &layerWidgets)
{
  QList<LayerWidget *> others;
  for (int i = 0; i < layout_->count(); ++i) {
    LayerWidget *lw = qobject_cast<LayerWidget *>(layout_->itemAt(i)->widget());
    if (!layerWidgets.contains(lw))
      others.append(lw);
  }
  select(layerWidgets);
  select(others, false);
}

void LayersPaneBase::selectExclusive(LayerWidget *layerWidget)
{
  selectExclusive(QList<LayerWidget *>() << layerWidget);
}

// Returns the indexes of the selected layers.
QList<int> LayersPaneBase::selectedPos() const
{
  QList<int> pos;
  const QList<QSharedPointer<Layer> > selLayers = layerMgr_->selectedLayers();

  for (int i = 0; i < layout_->count(); ++i) {
    LayerWidget *layerWidget = qobject_cast<LayerWidget *>(layout_->itemAt(i)->widget());
    if (selLayers.contains(layerWidget->layer()))
      pos.append(i);
  }

  return pos;
}

LayerWidget *LayersPaneBase::atPos(int pos)
{
  if (pos >= 0 && pos < layout_->count())
    return qobject_cast<LayerWidget *>(layout_->itemAt(pos)->widget());
  return 0;
}

void LayersPaneBase::remove(const QList<LayerWidget *> &layerWidgets, bool widgetsOnly, bool explicitRemove)
{
  QList<LayerWidget *> removableWidgets;
  foreach (LayerWidget *lw, layerWidgets)
    if (lw->isRemovable())
      removableWidgets.append(lw);

  if ((!removableWidgets.isEmpty()) && (!widgetsOnly) && explicitRemove &&
      (QMessageBox::warning(
         this, "Remove layers", QString("About to remove %1 layer%2; continue?").arg(removableWidgets.size()).arg(removableWidgets.size() != 1 ? "s" : ""),
         QMessageBox::Yes | QMessageBox::No) == QMessageBox::No))
    return;

  if (undoEnabled_ && explicitRemove) {
    QBitArray removableLayers(layerMgr_->orderedLayers().size());
    foreach (LayerWidget *lw, removableWidgets)
      removableLayers.setBit(layerMgr_->orderedLayers().indexOf(lw->layer()));
    EditItemManager::instance()->undoStack()->push(new RemoveLayersCommand(layerMgr_, removableLayers));
  } else {

    foreach (LayerWidget *lw, removableWidgets) {
      layout_->removeWidget(lw);
      if (!widgetsOnly)
        layerMgr_->removeLayer(lw->layer());
      delete lw;
    }

    // ensure to select the default layer if any
    if (!defaultLayer_.isNull()) {
      LayerWidget *lw = widgetFromLayer(defaultLayer_);
      if (lw)
        lw->setSelected();
    }

    if ((!removableWidgets.isEmpty()) && (!widgetsOnly))
      handleWidgetsUpdate();
  }
}

void LayersPaneBase::move(LayerWidget *layerWidget, bool up)
{
  const int index = layout_->indexOf(layerWidget);
  const int dstIndex = index + (up ? -1 : 1);
  if ((dstIndex < 0) || (dstIndex >= (layout_->count())))
    return;

  if (undoEnabled_) {
    EditItemManager::instance()->undoStack()->push(new MoveLayerCommand(layerMgr_, index, dstIndex));

  } else {

    const QSharedPointer<Layer> dstLayer = atPos(dstIndex)->layer();
    layout_->removeWidget(layerWidget);
    layout_->insertWidget(dstIndex, layerWidget);
    layerMgr_->moveLayer(layerWidget->layer(), dstLayer);
    ensureVisible(layerWidget);
    handleWidgetsUpdate();
  }
}

void LayersPaneBase::moveUp(LayerWidget *layerWidget)
{
  move(layerWidget, true);
}

void LayersPaneBase::moveUp(int index)
{
  moveUp(atPos(index));
}

void LayersPaneBase::moveSingleSelectedUp()
{
  const QList<int> selPos = selectedPos();
  if (selPos.size() == 1)
    moveUp(selPos.first());
}

void LayersPaneBase::moveDown(LayerWidget *layerWidget)
{
  move(layerWidget, false);
}

void LayersPaneBase::moveDown(int index)
{
  moveDown(atPos(index));
}

void LayersPaneBase::moveSingleSelectedDown()
{
  const QList<int> selPos = selectedPos();
  if (selPos.size() == 1)
    moveDown(selPos.first());
}

void LayersPaneBase::editAttrs(LayerWidget *layerWidget)
{
  layerWidget->editName(); // ### only the name attribute for now
}

void LayersPaneBase::editAttrsOfSingleSelected()
{
  const QList<int> selPos = selectedPos();
  if (selPos.size() != 1)
    return;

  LayerWidget *layerWidget = atPos(selPos.first());
  if (!layerWidget->isAttrsEditable())
    return;

  editAttrs(layerWidget);
}

QString LayersPaneBase::saveLayers(const QList<QSharedPointer<Layer> > &layers_, const QString &fileName) const
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QString error;
  KML::saveLayersToFile(fileName, layers_, &error);
  QApplication::restoreOverrideCursor();
  return error;
}

LayerWidget *LayersPaneBase::widgetFromLayer(const QSharedPointer<Layer> &layer)
{
  foreach (LayerWidget *lw, widgets())
    if (lw->layer() == layer)
      return lw;
  return 0;
}

QString LayersPaneBase::saveSelected(const QString &fileName) const
{
  return saveLayers(layers(selectedWidgets()), fileName);
}

void LayersPaneBase::mouseClicked(QMouseEvent *event)
{
  LayerWidget *layerWidget = qobject_cast<LayerWidget *>(sender());
  Q_ASSERT(layerWidget);

  if (event->button() & Qt::LeftButton) {
    if (event->modifiers().testFlag(Qt::NoModifier)) {
      // select target layer and unselect all others
      if (undoEnabled_) {
        QBitArray selected(layerMgr_->orderedLayers().size());
        selected.setBit(layerMgr_->orderedLayers().indexOf(layerWidget->layer()));
        if (selected != layerMgr_->selected())
          EditItemManager::instance()->undoStack()->push(new ModifyLayerSelectionCommand(layerMgr_, selected));
      } else {
        foreach (LayerWidget *lw, allWidgets()) {
          const bool selected = (lw == layerWidget);
          select(lw, selected);
        }
      }
    } else if (multiSelectable_ && (event->modifiers() & Qt::ControlModifier)) {
      // toggle selection of target layer if at least one other layer is selected, otherwise select target layer
      const QList<int> selPos = selectedPos();
      if (undoEnabled_) {
        QBitArray selected(layerMgr_->orderedLayers().size());
        for (int i = 0; i < layerMgr_->orderedLayers().size(); ++i) {
          const QSharedPointer<Layer> layer = layerMgr_->orderedLayers().at(i);
          selected.setBit(i, (layer == layerWidget->layer())
                           ? ((selPos.size() > 1) ? !layer->isSelected() : true)
                           : layer->isSelected());
        }
        if (selected != layerMgr_->selected())
          EditItemManager::instance()->undoStack()->push(new ModifyLayerSelectionCommand(layerMgr_, selected));
      } else {
        if (selPos.size() > 1)
          select(layerWidget, !layerWidget->layer()->isSelected());
        else
          select(layerWidget);
      }
    }
  } else if ((event->button() & Qt::RightButton) && event->modifiers().testFlag(Qt::NoModifier)
             && (layerWidget->layer()->isSelected())) {
    // open context menu and apply action to all selected layers

    QMenu contextMenu;

    // add common actions
    QAction moveUp_act(QPixmap(moveup_xpm), tr("Move Up"), 0);
    if (moveUpButton_) {
      moveUp_act.setIconVisibleInMenu(true);
      moveUp_act.setEnabled(moveUpButton_->isEnabled());
      contextMenu.addAction(&moveUp_act);
    }

    QAction moveDown_act(QPixmap(movedown_xpm), tr("Move Down"), 0);
    if (moveDownButton_) {
      moveDown_act.setIconVisibleInMenu(true);
      moveDown_act.setEnabled(moveDownButton_->isEnabled());
      contextMenu.addAction(&moveDown_act);
    }

    QAction editAttrs_act(QPixmap(edit_xpm), tr("Edit Attributes"), 0);
    editAttrs_act.setIconVisibleInMenu(true);
    editAttrs_act.setEnabled(editButton_->isEnabled());
    contextMenu.addAction(&editAttrs_act);

    addContextMenuActions(contextMenu); // add special actions

    QAction *action = contextMenu.exec(event->globalPos(), &moveUp_act);

    // try to match a special action
    if (handleContextMenuAction(action, selectedWidgets()))
      return;

    // try to match a common action
    if (action == &moveUp_act) {
      moveUp(layerWidget);
    } else if (action == &moveDown_act) {
      moveDown(layerWidget);
    } else if (action == &editAttrs_act) {
      editAttrs(layerWidget);
    }
  }
}

void LayersPaneBase::mouseDoubleClicked(QMouseEvent *event)
{
  if ((event->button() & Qt::LeftButton))
    editAttrsOfSingleSelected();
}

void LayersPaneBase::ensureVisibleTimeout()
{
  if (!visibleLayerWidget_)
    return;
  qApp->processEvents();
  scrollArea_->ensureWidgetVisible(visibleLayerWidget_);
  visibleLayerWidget_ = 0;
}

void LayersPaneBase::ensureVisible(LayerWidget *layerWidget)
{
  visibleLayerWidget_ = layerWidget;
  QTimer::singleShot(0, this, SLOT(ensureVisibleTimeout()));
}

void LayersPaneBase::setAllVisible(bool vis)
{
  if (undoEnabled_) {
    QBitArray visible(layerMgr_->orderedLayers().size(), vis);
    if (visible != layerMgr_->visible())
      EditItemManager::instance()->undoStack()->push(new ModifyLayerVisibilityCommand(layerMgr_, visible));
  } else {
    for (int i = 0; i < layout_->count(); ++i)
      qobject_cast<LayerWidget *>(layout_->itemAt(i)->widget())->setLayerVisible(vis);
    handleWidgetsUpdate();
  }
}

void LayersPaneBase::showAll()
{
  setAllVisible(true);
}

void LayersPaneBase::hideAll()
{
  setAllVisible(false);
}

void LayersPaneBase::handleVisibilityChanged(const QSharedPointer<Layer> &layer, bool vis)
{
  if (undoEnabled_) {
    QBitArray visible(layerMgr_->visible());
    visible.setBit(layerMgr_->orderedLayers().indexOf(layer), vis);
    if (visible != layerMgr_->visible())
      EditItemManager::instance()->undoStack()->push(new ModifyLayerVisibilityCommand(layerMgr_, visible));
  } else {
    layer->setVisible(vis);
    handleWidgetsUpdate();
  }
}

QList<LayerWidget *> LayersPaneBase::widgets(bool requireSelected, bool requireVisible) const
{
  QList<LayerWidget *> layerWidgets;
  for (int i = 0; i < layout_->count(); ++i) {
    LayerWidget *lw = qobject_cast<LayerWidget *>(layout_->itemAt(i)->widget());
    if ((!requireSelected || lw->layer()->isSelected()) && (!requireVisible || lw->isLayerVisible()))
      layerWidgets.append(lw);
  }
  return layerWidgets;
}

QList<LayerWidget *> LayersPaneBase::widgets(const QList<QSharedPointer<Layer> > &layers) const
{
  QList<LayerWidget *> layerWidgets;
  for (int i = 0; i < layout_->count(); ++i) {
    LayerWidget *lw = qobject_cast<LayerWidget *>(layout_->itemAt(i)->widget());
    if (layers.contains(lw->layer()))
      layerWidgets.append(lw);
  }
  return layerWidgets;
}

QList<LayerWidget *> LayersPaneBase::visibleWidgets() const
{
  return widgets(false, true);
}

QList<LayerWidget *> LayersPaneBase::selectedWidgets() const
{
  return widgets(true, false);
}

QList<LayerWidget *> LayersPaneBase::allWidgets() const
{
  return widgets(false, false);
}

QList<QSharedPointer<Layer> > LayersPaneBase::layers(const QList<LayerWidget *> &layerWidgets) const
{
  QList<QSharedPointer<Layer> > layers_;
  foreach (LayerWidget *layerWidget, layerWidgets)
    layers_.append(layerWidget->layer());
  return layers_;
}

void LayersPaneBase::updateButtons()
{
  int allCount;
  int selectedCount;
  int visibleCount;
  int removableCount;
  getLayerCounts(allCount, selectedCount, visibleCount, removableCount);
  const QList<int> selPos = selectedPos();

  if (showAllButton_ && hideAllButton_) {
    showAllButton_->setEnabled(visibleCount < allCount);
    hideAllButton_->setEnabled(visibleCount > 0);
  }
  moveUpButton_->setEnabled((selPos.size() == 1) && (selPos.first() > 0));
  moveDownButton_->setEnabled((selPos.size() == 1) && (selPos.first() < (allCount - 1)));
  editButton_->setEnabled((selPos.size() == 1) && atPos(selPos.first())->isAttrsEditable());
}

void LayersPaneBase::handleWidgetsUpdate()
{
  updateButtons();
  handleLayersUpdate();
}

// Updates widget structure (i.e. number and order of widgets).
void LayersPaneBase::updateWidgetStructure()
{
  QList<QSharedPointer<Layer> > activeLayers;
  foreach (const QSharedPointer<Layer> &layer, layerMgr_->orderedLayers()) {
    if (layer->isActive())
      activeLayers.append(layer);
  }

  // adjust widget list to match the number of layers
  while (layout_->count() > 0) {
    LayerWidget *lw = atPos(0);
    layout_->removeWidget(lw);
    delete lw;
  }
  for (int i = 0; i < activeLayers.size(); ++i) {
    LayerWidget *lw = new LayerWidget(layerMgr_, QSharedPointer<Layer>(), showInfo_, visibleLabelsEnabled_);
    layout_->insertWidget(0, lw);
  }

  // ### for now (remove when tested):
  if (layout_->count() != activeLayers.size())
    qFatal("ASSERTION FAILED: layout_->count()=%d != activeLayers.size()=%d",
           layout_->count(), activeLayers.size());

  // update widget contents
  for (int i = 0; i < activeLayers.size(); ++i) {
    LayerWidget *layerWidget = atPos(i);
    layerWidget->setState(activeLayers.at(i));
    initLayerWidget(layerWidget);
  }
}

void LayersPaneBase::setLayerUpdatesEnabled(bool enabled)
{
  layerUpdatesEnabled_ = enabled;
}

void LayersPaneBase::handleLayersUpdate()
{
  if (!layerUpdatesEnabled_)
    return;
  emit updated();
}

void LayersPaneBase::update()
{
  updateWidgetStructure();
  updateButtons();
  handleLayersUpdate();
}

void LayersPaneBase::getLayerCounts(int &allCount, int &selectedCount, int &visibleCount, int &removableCount) const
{
  allCount = selectedCount = visibleCount = removableCount = 0;
  foreach (LayerWidget *lw, allWidgets()) {
    allCount++;
    if (lw->layer()->isSelected()) {
      selectedCount++;
      if (lw->isRemovable())
        removableCount++;
    }
    if (lw->isLayerVisible())
      visibleCount++;
  }
}

void LayersPaneBase::getSelectedLayersItemCounts(int &allCount, int &selectedCount) const
{
  allCount = selectedCount = 0;
  foreach (LayerWidget *lw, selectedWidgets()) {
    allCount += lw->layer()->itemCount();
    selectedCount += lw->layer()->selectedItemCount();
  }
}

} // namespace
