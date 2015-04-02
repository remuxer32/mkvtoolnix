#include "common/common_pch.h"

#include "common/qt.h"
#include "mkvtoolnix-gui/jobs/mux_job.h"
#include "mkvtoolnix-gui/jobs/tool.h"
#include "mkvtoolnix-gui/main_window/main_window.h"
#include "mkvtoolnix-gui/merge/command_line_dialog.h"
#include "mkvtoolnix-gui/merge/tool.h"
#include "mkvtoolnix-gui/forms/main_window.h"
#include "mkvtoolnix-gui/forms/merge/tool.h"
#include "mkvtoolnix-gui/util/option_file.h"
#include "mkvtoolnix-gui/util/settings.h"
#include "mkvtoolnix-gui/util/util.h"

#include <QComboBox>
#include <QMenu>
#include <QTreeView>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QString>
#include <QTimer>

namespace mtx { namespace gui { namespace Merge {

Tool::Tool(QWidget *parent,
           QMenu *mergeMenu)
  : ToolBase{parent}
  , ui{new Ui::Tool}
  , m_mergeMenu{mergeMenu}
  , m_filesModel{new SourceFileModel{this}}
  , m_tracksModel{new TrackModel{this}}
  , m_currentlySettingInputControlValues{false}
  , m_addFilesAction{new QAction{this}}
  , m_appendFilesAction{new QAction{this}}
  , m_addAdditionalPartsAction{new QAction{this}}
  , m_removeFilesAction{new QAction{this}}
  , m_removeAllFilesAction{new QAction{this}}
  , m_attachmentsModel{new AttachmentModel{this, m_config.m_attachments}}
  , m_addAttachmentsAction{new QAction{this}}
  , m_removeAttachmentsAction{new QAction{this}}
{
  // Setup UI controls.
  ui->setupUi(this);

  m_filesModel->setTracksModel(m_tracksModel);

  setupMenu();
  setupInputControls();
  setupOutputControls();
  setupAttachmentsControls();

  setControlValuesFromConfig();
}

Tool::~Tool() {
}

void
Tool::onShowCommandLine() {
  auto options = (QStringList{} << Settings::get().actualMkvmergeExe()) + m_config.buildMkvmergeOptions();
  CommandLineDialog{this, options, QY("mkvmerge command line")}.exec();
}

void
Tool::onSaveConfig() {
  if (m_config.m_configFileName.isEmpty())
    onSaveConfigAs();
  else {
    m_config.save();
    MainWindow::get()->setStatusBarMessage(QY("The configuration has been saved."));
  }
}

void
Tool::onSaveOptionFile() {
  auto fileName = QFileDialog::getSaveFileName(this, QY("Save option file"), Settings::get().m_lastConfigDir.path(), QY("All files") + Q(" (*)"));
  if (fileName.isEmpty())
    return;

  OptionFile::create(fileName, m_config.buildMkvmergeOptions());
  Settings::get().m_lastConfigDir = QFileInfo{fileName}.path();
  Settings::get().save();

  MainWindow::get()->setStatusBarMessage(QY("The option file has been created."));
}

void
Tool::onSaveConfigAs() {
  auto fileName = QFileDialog::getSaveFileName(this, QY("Save settings file as"), Settings::get().m_lastConfigDir.path(), QY("MKVToolNix GUI config files") + Q(" (*.mtxcfg);;") + QY("All files") + Q(" (*)"));
  if (fileName.isEmpty())
    return;

  m_config.save(fileName);
  Settings::get().m_lastConfigDir = QFileInfo{fileName}.path();
  Settings::get().save();

  MainWindow::get()->setStatusBarMessage(QY("The configuration has been saved."));
}

void
Tool::onOpenConfig() {
  auto fileName = QFileDialog::getOpenFileName(this, QY("Open settings file"), Settings::get().m_lastConfigDir.path(), QY("MKVToolNix GUI config files") + Q(" (*.mtxcfg);;") + QY("All files") + Q(" (*)"));
  if (fileName.isEmpty())
    return;

  Settings::get().m_lastConfigDir = QFileInfo{fileName}.path();
  Settings::get().save();

  try {
    m_config.load(fileName);
    MainWindow::get()->setStatusBarMessage(QY("The configuration has been loaded."));

  } catch (InvalidSettingsX &) {
    m_config.reset();
    QMessageBox::critical(this, QY("Error loading settings file"), QY("The settings file '%1' contains invalid settings and was not loaded.").arg(fileName));
  }

  setControlValuesFromConfig();
}

void
Tool::onNew() {
  m_config.reset();
  setControlValuesFromConfig();
  MainWindow::get()->setStatusBarMessage(QY("The configuration has been reset."));
}

void
Tool::onAddToJobQueue() {
  addToJobQueue(false);
}

void
Tool::onStartMuxing() {
  addToJobQueue(true);
}

QString
Tool::getOpenFileName(QString const &title,
                      QString const &filter,
                      QLineEdit *lineEdit) {
  auto fullFilter = filter;
  if (!fullFilter.isEmpty())
    fullFilter += Q(";;");
  fullFilter += QY("All files") + Q(" (*)");

  auto dir      = lineEdit->text().isEmpty() ? Settings::get().m_lastOpenDir.path() : QFileInfo{ lineEdit->text() }.path();
  auto fileName = QFileDialog::getOpenFileName(this, title, dir, fullFilter);
  if (fileName.isEmpty())
    return fileName;

  Settings::get().m_lastOpenDir = QFileInfo{fileName}.path();
  Settings::get().save();

  lineEdit->setText(fileName);

  return fileName;
}

QString
Tool::getSaveFileName(QString const &title,
                      QString const &filter,
                      QLineEdit *lineEdit) {
  auto fullFilter = filter;
  if (!fullFilter.isEmpty())
    fullFilter += Q(";;");
  fullFilter += QY("All files") + Q(" (*)");

  auto dir      = lineEdit->text().isEmpty() ? Settings::get().m_lastOutputDir.path() : QFileInfo{ lineEdit->text() }.path();
  auto fileName = QFileDialog::getSaveFileName(this, title, dir, fullFilter);
  if (fileName.isEmpty())
    return fileName;

  Settings::get().m_lastOutputDir = QFileInfo{fileName}.path();
  Settings::get().save();

  lineEdit->setText(fileName);

  return fileName;
}

void
Tool::setupMenu() {
  auto mwUi = MainWindow::get()->getUi();

  connect(mwUi->actionMergeNew,                     SIGNAL(triggered()), this, SLOT(onNew()));
  connect(mwUi->actionMergeOpen,                    SIGNAL(triggered()), this, SLOT(onOpenConfig()));
  connect(mwUi->actionMergeSave,                    SIGNAL(triggered()), this, SLOT(onSaveConfig()));
  connect(mwUi->actionMergeSaveAs,                  SIGNAL(triggered()), this, SLOT(onSaveConfigAs()));
  connect(mwUi->actionMergeSaveOptionFile,          SIGNAL(triggered()), this, SLOT(onSaveOptionFile()));
  connect(mwUi->actionMergeStartMuxing,             SIGNAL(triggered()), this, SLOT(onStartMuxing()));
  connect(mwUi->actionMergeAddToJobQueue,           SIGNAL(triggered()), this, SLOT(onAddToJobQueue()));
  connect(mwUi->actionMergeShowMkvmergeCommandLine, SIGNAL(triggered()), this, SLOT(onShowCommandLine()));
}

void
Tool::setControlValuesFromConfig() {
  m_filesModel->setSourceFiles(m_config.m_files);
  m_tracksModel->setTracks(m_config.m_tracks);
  m_attachmentsModel->setAttachments(m_config.m_attachments);

  resizeFilesColumnsToContents();
  resizeTracksColumnsToContents();
  resizeAttachmentsColumnsToContents();

  onFileSelectionChanged();
  onTrackSelectionChanged();
  setOutputControlValues();
  onAttachmentSelectionChanged();
}

void
Tool::retranslateUi() {
  retranslateInputUI();
  retranslateAttachmentsUI();
}

bool
Tool::isReadyForMerging() {
  if (m_config.m_files.isEmpty()) {
    QMessageBox::critical(this, QY("Cannot start merging"), QY("You have to add at least one source file before you can start merging or add a job to the job queue."));
    return false;
  }

  if (m_config.m_destination.isEmpty()) {
    QMessageBox::critical(this, QY("Cannot start merging"), QY("You have to set the output file name before you can start merging or add a job to the job queue."));
    return false;
  }

  return true;
}

void
Tool::addToJobQueue(bool startNow) {
  if (!isReadyForMerging())
    return;

  auto newConfig     = std::make_shared<MuxConfig>(m_config);
  auto job           = std::make_shared<mtx::gui::Jobs::MuxJob>(startNow ? mtx::gui::Jobs::Job::PendingAuto : mtx::gui::Jobs::Job::PendingManual, newConfig);
  job->m_dateAdded   = QDateTime::currentDateTime();
  job->m_description = job->displayableDescription();

  if (!startNow) {
    auto newDescription = QString{};

    while (newDescription.isEmpty()) {
      bool ok = false;
      newDescription = QInputDialog::getText(this, QY("Enter job description"), QY("Please enter the new job's description."), QLineEdit::Normal, job->m_description, &ok);
      if (!ok)
        return;
    }

    job->m_description = newDescription;
  }

  MainWindow::getJobTool()->addJob(std::static_pointer_cast<mtx::gui::Jobs::Job>(job));
}

void
Tool::toolShown() {
  MainWindow::get()->showTheseMenusOnly({ m_mergeMenu });
}

}}}