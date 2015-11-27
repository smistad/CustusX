#ifndef CXTREEITEMMODEL_H
#define CXTREEITEMMODEL_H

#include "cxBaseWidget.h"

#include <QtGui>
#include "cxImage.h"
#include "cxTreeNode.h"
#include "cxStringProperty.h"
#include "cxXmlOptionItem.h"
#include "cxLabeledComboBoxWidget.h"

class QTreeWidget;
class QVBoxLayout;
class QTreeWidgetItem;
class QTreeView;

namespace cx
{

typedef boost::shared_ptr<class TreeRepository> TreeRepositoryPtr;

/**
 * \ingroup cxGUI
 *
 */
class TreeItemModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  TreeItemModel(XmlOptionFile options, VisServicesPtr services, QObject* parent = 0);
  virtual ~TreeItemModel();
  void setSelectionModel(QItemSelectionModel* selectionModel);

  virtual int columnCount (const QModelIndex& parent = QModelIndex() ) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;
  virtual bool setData(const QModelIndex& index, const QVariant& value, int role);
  virtual Qt::ItemFlags flags(const QModelIndex& index ) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex() ) const;
  virtual QModelIndex parent(const QModelIndex& index ) const;

  TreeNodePtr getCurrentItem();
  TreeRepositoryPtr repo() { return mRepository; }

signals:
  void hasBeenReset();
  void currentItemChanged();

public slots:
  void update();

private slots:
  void currentItemChangedSlot(const QModelIndex& current, const QModelIndex& previous);
  void onRepositoryChanged();

private:
  int mViewGroupCount;
  TreeRepositoryPtr mRepository;
  QItemSelectionModel* mSelectionModel;
  XmlOptionFile mOptions;
  VisServicesPtr mServices;

  TreeNode* itemFromIndex(const QModelIndex& index) const;
  void fillModelTreeFromViewManager(TreeNodePtr root);
};

}//end namespace cx


#endif // CXTREEITEMMODEL_H