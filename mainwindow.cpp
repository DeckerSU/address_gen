#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QCryptographicHash>
#include <bitcoin/system.hpp>

// http://aaronjaramillo.org/libbitcoin-first-program
// https://github.com/libbitcoin/libbitcoin-system/wiki/Examples-from-Addresses-and-HD-Wallets

// Namespace
using namespace libbitcoin::system;

typedef struct prefix {
    const char *name;
    unsigned char pubkey_prefix[4];
    size_t pubkey_prefix_size;
    unsigned char privkey_prefix[4];
    size_t privkey_prefix_size;
} prefix_t;

prefix_t prefixes[] = {
    { "BTC",               { 0 }, 1,  { 128 }, 1 },
    { "KMD",               { 60 }, 1,  { 188 }, 1 },
    { "ZEC",               {0x1C,0xB8}, 2, {0x80}, 1 },
    { "GAME",               {38}, 1, {166}, 1 },
    { "EMC2",               {33}, 1, {176}, 1 },
    { "GIN",                {38}, 1, {198}, 1 },
    { "SUQA",               {63}, 1, {191}, 1 },
    { NULL, {}, 0 , {}, 0 }
};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->setWindowTitle("Passphrase2Address GUI");
    QStringList keys;
    keys << "Coin" << "Comp. address" << "WIF"; // << "Privkey";
    ui->tableWidgetAddresses->clear();
    ui->tableWidgetAddresses->setRowCount(0);
    ui->tableWidgetAddresses->setColumnCount(keys.size());
    ui->tableWidgetAddresses->setHorizontalHeaderLabels(keys);
    ui->tableWidgetAddresses->resizeColumnsToContents();
    ui->tableWidgetAddresses->horizontalHeader()->setStretchLastSection(true);

    ui->lineEdit->setText("myverysecretandstrongpassphrase_noneabletobrute");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonGenerate_clicked()
{
    int row;

    QString s = ui->lineEdit->text();
    QByteArray hash = QCryptographicHash::hash(s.toLocal8Bit(),QCryptographicHash::Sha256);

    hash[0]  = hash[0] & 248;
    hash[31] = hash[31] & 127;
    hash[31] = hash[31] | 64;

    QString hash_str = hash.toHex();

    /*
    qDebug() << "Passphrase: " << s;
    qDebug() << "Hash: " << hash_str;
    */

    ui->textBrowserLog->append("Passphrase: " + s);

    // Begin with a private key
    // auto my_secret = base16_literal("f3c8f9a6198cca98f481edde13bcc031b1470a81e367b838fe9e0a9db0f5993d");

    std::string hexKey = hash_str.toStdString();
    ec_secret my_secret;
    decode_base16(my_secret, hexKey);

    /*
    hexKey = encode_base16(my_secret);
    qDebug() << QString::fromStdString(hexKey);
    */

    // Derive pubkey point
    ec_compressed my_pubkey;
    secret_to_public(my_pubkey, my_secret);

    std::string pubkeyhex_str = encode_base16(my_pubkey);
    ui->textBrowserLog->append("Pubkey (hex): " + QString::fromStdString(pubkeyhex_str));
    ui->textBrowserLog->append("Privkey (hex): " + hash_str);

    ui->tableWidgetAddresses->clearContents();
    ui->tableWidgetAddresses->setRowCount(0);

    int idx = 0;
    prefix_t current_prefix = prefixes[idx];
    do {

        ui->tableWidgetAddresses->insertRow( ui->tableWidgetAddresses->rowCount() );
        row = ui->tableWidgetAddresses->rowCount()-1;

        // Pubkeyhash: sha256 + hash160
        auto my_pubkeyhash = bitcoin_short_hash(my_pubkey);

        one_byte addr_prefix;
        std::string addr;

        if (current_prefix.pubkey_prefix_size == 1) {
            addr_prefix = { { current_prefix.pubkey_prefix[0] } };
            // Byte sequence = prefix + pubkey + checksum(4-bytes)
            data_chunk prefix_pubkey_checksum(to_chunk(addr_prefix));
            extend_data(prefix_pubkey_checksum, my_pubkeyhash);
            append_checksum(prefix_pubkey_checksum);
            // Base58 encode byte sequence -> Bitcoin Address
            addr = encode_base58(prefix_pubkey_checksum);
        } else {
            one_byte ob;
            ob = { { current_prefix.pubkey_prefix[0] } };
            auto prefix_pubkey_checksum = to_chunk(ob);
            ob = { { current_prefix.pubkey_prefix[1] } };
            extend_data(prefix_pubkey_checksum, ob);
            extend_data(prefix_pubkey_checksum, my_pubkeyhash);
            append_checksum(prefix_pubkey_checksum);
            addr = encode_base58(prefix_pubkey_checksum);
        }

        // qDebug() << QString::fromStdString(addr);

        // You can directly generate Bitcoin addresses
        // with Libbitcoin wallet types: ec_private/ec_public
        // described in the following section

        one_byte secret_prefix = { { current_prefix.privkey_prefix[0] } };
        one_byte secret_compressed = { { 0x01 } }; // omitted if uncompressed
        // Apply prefix, suffix & append checksum
        auto prefix_secret_comp_checksum = to_chunk(secret_prefix);
        extend_data(prefix_secret_comp_checksum, my_secret);
        extend_data(prefix_secret_comp_checksum, secret_compressed);
        append_checksum(prefix_secret_comp_checksum);
        // WIF (mainnet/compressed)
        //qDebug() << QString::fromStdString(encode_base58(prefix_secret_comp_checksum));

        ui->tableWidgetAddresses->setItem( row , 0, new QTableWidgetItem(current_prefix.name));
        ui->tableWidgetAddresses->setItem( row , 1, new QTableWidgetItem(QString::fromStdString(addr)));
        ui->tableWidgetAddresses->setItem( row , 2, new QTableWidgetItem( QString::fromStdString(encode_base58(prefix_secret_comp_checksum)) ));
        //ui->tableWidgetAddresses->setItem( row , 3, new QTableWidgetItem(hash_str));

        idx ++;
        current_prefix = prefixes[idx];

    } while (current_prefix.name != NULL);


    ui->tableWidgetAddresses->resizeColumnsToContents();


}
