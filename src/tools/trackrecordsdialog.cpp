#include "trackrecordsdialog.h"
#include "ui_trackrecordsdialog.h"

#include "../core/trackrecords.h"

bool loadingRecords = false;

TrackRecordsDialog::TrackRecordsDialog(QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::TrackRecordsDialog), currentIndex(0), currentTV(0), currentTWR(0)
{
    ui->setupUi(this);
    drDialog = new DriverRecordsDialog(this);
}

TrackRecordsDialog::~TrackRecordsDialog()
{
    delete ui;
}

void TrackRecordsDialog::exec()
{
    TrackRecords &tr = TrackRecords::getInstance();
    QStringList sList = tr.getTrackList();
    if (ui->listWidget->count() != sList.size())
    {
        for (int i = 0; i < sList.size(); ++i)
        {
            QListWidgetItem *item = ui->listWidget->item(i);
            if (!item)
            {
                item = new QListWidgetItem();
                ui->listWidget->insertItem(i, item);
            }
            item->setText(sList[i]);
        }

        for (int i = ui->listWidget->count() - 1; i >= tr.getTrackRecords().size(); --i)
        {
            QListWidgetItem *item = ui->listWidget->item(i);
            ui->listWidget->removeItemWidget(item);
        }                
    }
    loadTrackRecords();
    QDialog::show();
}

void TrackRecordsDialog::loadTrackRecords()
{
    loadingRecords = true;

    TrackVersion *tv = currentTV;
    TrackWeekendRecords *twr = currentTWR;
    Track *tr = 0;

    if (currentIndex == -1)
    {
        TrackRecords::getInstance().getCurrentTrackRecords(&twr, &tv);

        if (twr == 0 || tv == 0)
        {
            if (TrackRecords::getInstance().getTrackRecords().isEmpty())
                return;

            tr = &TrackRecords::getInstance().getTrackRecords().first();
            tv = &(*tr)[0];
            twr = &(*tv)[0];
        }
    }
    else
    {
        tr = &TrackRecords::getInstance()[currentIndex];
        if (tv == 0 || twr == 0)
        {
            tv = &(*tr)[0];
            twr = &(*tv)[0];
        }
    }

    setWindowTitle("Track records: " + tr->name);

    ui->trackNameLabel->setText(tr->name);
    ui->trackMapLabel->setPixmap(tv->map);

    ui->trackVersionBox->clear();
    for (int i = 0; i < tr->trackVersions.size(); ++i)
        ui->trackVersionBox->addItem(QString::number(tr->trackVersions[i].year));

    int idx = ui->trackVersionBox->findText(QString::number(tv->year));
    if (idx != -1)
        ui->trackVersionBox->setCurrentIndex(idx);
    else
        ui->trackVersionBox->setCurrentIndex(0);

    ui->yearBox->clear();
    for (int i = 0; i < tv->trackWeekendRecords.size(); ++i)
        ui->yearBox->addItem(QString::number(tv->trackWeekendRecords[i].year));

    idx = ui->yearBox->findText(QString::number(twr->year));
    if (idx != -1)
        ui->yearBox->setCurrentIndex(idx);
    else
        ui->yearBox->setCurrentIndex(0);

    ui->qRTLabel->setText(tv->trackRecords[QUALI_RECORD].time.toString());
    ui->qRDLabel->setText(tv->trackRecords[QUALI_RECORD].driver);
    ui->qRDTLabel->setText(tv->trackRecords[QUALI_RECORD].team);
    ui->qRYLabel->setText(tv->trackRecords[QUALI_RECORD].year);

    ui->rRTLabel->setText(tv->trackRecords[RACE_RECORD].time.toString());
    ui->rRDLabel->setText(tv->trackRecords[RACE_RECORD].driver);
    ui->rRDTLabel->setText(tv->trackRecords[RACE_RECORD].team);
    ui->rRYLabel->setText(tv->trackRecords[RACE_RECORD].year);

    ui->s1TLabel->setText(twr->sessionRecords[S1_RECORD].time.toString());
    ui->s1DLabel->setText(twr->sessionRecords[S1_RECORD].driver);
    ui->s1DTLabel->setText(twr->sessionRecords[S1_RECORD].team);
    ui->s1SLabel->setText(twr->sessionRecords[S1_RECORD].session);

    ui->s2TLabel->setText(twr->sessionRecords[S2_RECORD].time.toString());
    ui->s2DLabel->setText(twr->sessionRecords[S2_RECORD].driver);
    ui->s2DTLabel->setText(twr->sessionRecords[S2_RECORD].team);
    ui->s2SLabel->setText(twr->sessionRecords[S2_RECORD].session);

    ui->s3TLabel->setText(twr->sessionRecords[S3_RECORD].time.toString());
    ui->s3DLabel->setText(twr->sessionRecords[S3_RECORD].driver);
    ui->s3DTLabel->setText(twr->sessionRecords[S3_RECORD].team);
    ui->s3SLabel->setText(twr->sessionRecords[S3_RECORD].session);

    ui->tTLabel->setText(twr->sessionRecords[TIME_RECORD].time.toString());
    ui->tDLabel->setText(twr->sessionRecords[TIME_RECORD].driver);
    ui->tDTLabel->setText(twr->sessionRecords[TIME_RECORD].team);
    ui->tSLabel->setText(twr->sessionRecords[TIME_RECORD].session);

    currentTV = tv;
    currentTWR = twr;

    loadingRecords = false;
}

void TrackRecordsDialog::on_listWidget_clicked(const QModelIndex &index)
{
    currentIndex = index.row();
    currentTV = 0;
    currentTWR = 0;
    loadTrackRecords();

    if (currentTWR == 0)
        return;

    if (drDialog->isVisible())
    {
        QString name;
        if (currentIndex > -1)
            name = TrackRecords::getInstance()[currentIndex].name;
        drDialog->loadRecords(*currentTWR, name);
    }
}

void TrackRecordsDialog::on_toolButton_clicked()
{
    if (currentIndex > 0)
    {
        --currentIndex;
        currentTV = 0;
        currentTWR = 0;
        ui->listWidget->setCurrentRow(currentIndex);
        loadTrackRecords();
    }
}

void TrackRecordsDialog::on_toolButton_2_clicked()
{
    if (currentIndex < TrackRecords::getInstance().getTrackRecords().size() - 1)
    {
        ++currentIndex;
        currentTV = 0;
        currentTWR = 0;
        ui->listWidget->setCurrentRow(currentIndex);
        loadTrackRecords();
    }
}

void TrackRecordsDialog::saveSettings(QSettings &settings)
{
    settings.setValue("ui/trackrecords_geometry", saveGeometry());
    settings.setValue("ui/trackrecords_splitter", ui->splitter->saveState());

    drDialog->saveSettings(settings);

}

void TrackRecordsDialog::loadSettings(QSettings &settings)
{
    restoreGeometry(settings.value("ui/trackrecords_geometry").toByteArray());
    ui->splitter->restoreState(settings.value("ui/trackrecords_splitter").toByteArray());

    drDialog->loadSettings(settings);
}

void TrackRecordsDialog::setFont(const QFont &font)
{
    for (int i = 0; i < ui->gridLayout_4->count(); ++i)
    {
        ui->gridLayout_4->itemAt(i)->widget()->setFont(font);
    }
    for (int i = 0; i < ui->gridLayout_2->count(); ++i)
    {
        ui->gridLayout_2->itemAt(i)->widget()->setFont(font);
    }

    ui->groupBox->setFont(font);
    ui->recordsLabel->setFont(font);
    ui->label->setFont(font);

    drDialog->setFont(font);
}

void TrackRecordsDialog::on_pushButton_clicked()
{
    if (currentIndex == -1 || currentTWR == 0)
        return;

    TrackRecords::getInstance().gatherSessionRecords(true);


    QString name = TrackRecords::getInstance()[currentIndex].name;
    drDialog->exec(*currentTWR, name);
}

void TrackRecordsDialog::update()
{
    if (currentIndex == -1)
        return;

    Track &tr = TrackRecords::getInstance()[currentIndex];

    if (tr.name == EventData::getInstance().getEventInfo().eventPlace)
    {
        TrackRecords::getInstance().gatherSessionRecords(true);
        loadTrackRecords();

        qDebug() << "UPDATE";

        if (drDialog->isVisible())
        {
            QString name = TrackRecords::getInstance()[currentIndex].name;
            drDialog->loadRecords(*currentTWR, name);
        }
    }
}

void TrackRecordsDialog::on_yearBox_currentIndexChanged(const QString &arg1)
{
    if (!loadingRecords && currentTV != 0)
    {
        currentTWR = &currentTV->getTrackWeekendRecords(arg1.toInt());
        if (*currentTWR != TrackWeekendRecords::null())
            loadTrackRecords();
        else
            currentTWR = 0;

    }
}

void TrackRecordsDialog::on_trackVersionBox_activated(const QString &arg1)
{
    if (!loadingRecords && currentIndex != -1)
    {
        currentTV = &TrackRecords::getInstance()[currentIndex].getTrackVersion(arg1.toInt());
        if (*currentTV != TrackVersion::null())
        {
            currentTWR = &(*currentTV)[0];
            loadTrackRecords();
        }
        else
        {
            currentTV = 0;
            currentTWR = 0;
        }
    }
}