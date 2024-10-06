// Inclusão dos arquivos de cabeçalho necessários
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>
#include <QDate>
#include <vector>
#include <QMessageBox>

// Variável estática para gerar IDs únicos para cada paciente
static int id_autoincremental = 0;

// Enumeração para representar os diferentes sintomas usando flags binárias
typedef enum{
    TOSSE = 1,
    NAUSEA = 2,
    FEBRE = 4,
    DISENTERIA = 8,
    INFECCAO = 16,
    COVID = 32,
    AVC = 64,
    ACIDENTE = 128
} SINTOMAS;

// Classe Patient para representar os pacientes
class Patient
{
public:
    /**
     * @brief Construtor da classe Patient.
     * @param name Nome do paciente.
     * @param cpf CPF do paciente.
     * @param email Email do paciente.
     * @param bday Data de nascimento do paciente.
     * @param sintomas Sintomas do paciente representados por flags binárias.
     */
    Patient(QString name, QString cpf, QString email, QDate bday, unsigned sintomas);

    int id;             // Identificador único do paciente
    QString name;       // Nome do paciente
    QString cpf;        // CPF do paciente
    QString email;      // Email do paciente
    QDate bday;         // Data de nascimento do paciente
    int idade;          // Idade do paciente
    unsigned sintomas;  // Sintomas do paciente representados por flags binárias
};

// Implementação do construtor da classe Patient
Patient::Patient(QString name, QString cpf, QString email, QDate bday, unsigned sintomas ){
    QDate current_date = QDate::currentDate();          // Obtém a data atual
    this->id = id_autoincremental++;                    // Atribui um ID único e incrementa o contador
    this->name = name;                                  // Define o nome do paciente
    this->cpf = cpf;                                    // Define o CPF do paciente
    this->email = email;                                // Define o email do paciente
    this->bday = bday;                                  // Define a data de nascimento
    this->sintomas = sintomas;                          // Define os sintomas usando flags binárias
    this->idade = current_date.year() - bday.year();    // Calcula a idade básica

    // Ajusta a idade se o aniversário ainda não ocorreu no ano atual
    if (current_date.month() < bday.month() ||
        (current_date.month() == bday.month() && current_date.day() < bday.day())) {
        this->idade--;
    }
}

// Vetores para gerenciar a fila de pacientes, fila ordenada por nome e relatório de pacientes tratados
std::vector<Patient> fila;
std::vector<Patient> sorted_name_fila;
std::vector<Patient> relatorio;

// Variável para controlar o tempo de tratamento do paciente atual (-1 indica que nenhum tratamento está em andamento)
int tempo_tratamento = -1;

/**
 * @brief Construtor da classe MainWindow.
 * Configura a interface do usuário e inicializa os temporizadores.
 * @param parent Widget pai.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);            // Configura a interface do usuário
    ui->progressBar->setValue(0); // Inicializa a barra de progresso com valor 0

    // Cria um temporizador para atualizar a interface do usuário a cada intervalo padrão
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_ui()));
    timer->start(); // Inicia o temporizador

    // Cria um segundo temporizador para atualizar a fila a cada 1 segundo (1000 milissegundos)
    QTimer *timer1sec = new QTimer(this);
    connect(timer1sec, SIGNAL(timeout()), this, SLOT(update_fila()));
    timer1sec->start(1000); // Inicia o temporizador com intervalo de 1 segundo
}

/**
 * @brief Atualiza a interface com a hora e data atuais.
 * Não possui parâmetros de entrada ou retorno.
 */
void MainWindow::update_ui()
{
    QTime time = QTime::currentTime();                // Obtém a hora atual
    QString time_to_text = time.toString("hh:mm:ss");  // Formata a hora como string
    QDate date = QDate::currentDate();                // Obtém a data atual
    QString date_to_text = date.toString();            // Converte a data para string
    ui->clock->setText(time_to_text);                 // Atualiza o label do relógio na interface
    ui->date->setText(date_to_text);                  // Atualiza o label da data na interface
}

/**
 * @brief Atualiza a fila de pacientes na interface e gerencia o tratamento atual.
 * Não possui parâmetros de entrada ou retorno.
 */
void MainWindow::update_fila()
{
    const QString default_value = "Aguardando paciente"; // Valor padrão quando não há pacientes na fila

    // Verifica se a fila está vazia
    if(fila.empty()){
        // Define todos os campos de pacientes na interface com o valor padrão
        ui->primeiro->setText(default_value);
        ui->segundo->setText(default_value);
        ui->terceiro->setText(default_value);
        ui->quarto->setText(default_value);
        ui->ultimo->setText(default_value);
        return; // Sai da função se não há pacientes
    }

    // Atualiza os campos de pacientes na interface com os CPFs dos pacientes na fila
    ui->primeiro->setText(fila.size() > 0 ? fila.at(0).cpf : default_value);
    ui->segundo->setText(fila.size() > 1 ? fila.at(1).cpf : default_value);
    ui->terceiro->setText(fila.size() > 2 ? fila.at(2).cpf : default_value);
    ui->quarto->setText(fila.size() > 3 ? fila.at(3).cpf : default_value);
    ui->ultimo->setText(fila.size() > 4 ? fila.at(4).cpf : default_value);

    // Gerencia o tratamento do paciente atual
    if(tempo_tratamento == -1){
        // Se nenhum tratamento está em andamento, inicia o tratamento do primeiro paciente da fila
        QString chamado = QString(fila.at(0).cpf + " " + QString::number(fila.at(0).id));
        ui->numero_chamado->setText(chamado);          // Atualiza o campo de chamado com CPF e ID do paciente
        tempo_tratamento = fila.at(0).sintomas + 1;    // Define o tempo de tratamento baseado nos sintomas
        ui->progressBar->setValue(0);                  // Reseta a barra de progresso
    }
    else if(tempo_tratamento > 0){
        // Se o tratamento está em andamento, decrementa o tempo restante
        tempo_tratamento--;
        int porcentagem = (fila.at(0).sintomas - tempo_tratamento) * 100 / fila.at(0).sintomas; // Calcula a porcentagem de conclusão
        ui->progressBar->setValue(porcentagem); // Atualiza a barra de progresso

        // Verifica se o tratamento foi concluído
        if(tempo_tratamento == 0){
            tempo_tratamento = -1;           // Reseta o tempo de tratamento
            ui->progressBar->setValue(0);    // Reseta a barra de progresso
            fila.erase(fila.begin());         // Remove o paciente tratado da fila
        }
    }
}

/**
 * @brief Gera um relatório dos pacientes tratados e exibe no TextBrowser.
 * Não possui parâmetros de entrada ou retorno.
 */
void MainWindow::gerar_relatorio() {
    for (const Patient& patient : relatorio) {
        QString info;
        info += "ID: " + QString::number(patient.id) + "\n";                        // Adiciona o ID do paciente
        info += "Nome: " + patient.name + "\n";                                      // Adiciona o nome do paciente
        info += "CPF: " + patient.cpf + "\n";                                        // Adiciona o CPF do paciente
        info += "Email: " + patient.email + "\n";                                    // Adiciona o email do paciente
        info += "Data de Nascimento: " + patient.bday.toString("dd/MM/yyyy") + "\n"; // Adiciona a data de nascimento
        info += "Idade: " + QString::number(patient.idade) + "\n";                   // Adiciona a idade do paciente
        info += "Sintomas: ";                                                        // Início da lista de sintomas

        // Verifica e adiciona os sintomas do paciente
        if (patient.sintomas & TOSSE)       info += "TOSSE ";
        if (patient.sintomas & NAUSEA)      info += "NÁUSEA ";
        if (patient.sintomas & FEBRE)       info += "FEBRE ";
        if (patient.sintomas & DISENTERIA)  info += "DISENTERIA ";
        if (patient.sintomas & INFECCAO)    info += "INFECÇÃO ";
        if (patient.sintomas & COVID)       info += "COVID ";
        if (patient.sintomas & AVC)         info += "AVC ";
        if (patient.sintomas & ACIDENTE)    info += "ACIDENTE ";

        info += "\n-----------------------------\n"; // Adiciona uma linha separadora

        // Adiciona as informações ao TextBrowser na interface
        ui->textBrowser->append(info);
    }
}

/**
 * @brief Ordena a fila de pacientes com base nos sintomas usando o algoritmo de seleção.
 * Não possui parâmetros de entrada ou retorno.
 */
void ordenar_paciente_sintoma(){
    int n = fila.size(); // Obtém o tamanho da fila

    // Loop externo para cada posição na fila
    for (int i = 0; i < n - 1; ++i) {
        int max_idx = i;                     // Assume que o primeiro elemento é o de maior prioridade
        int max_priority = fila[i].sintomas; // Obtém a prioridade atual

        // Loop interno para encontrar o paciente com a maior prioridade na parte não ordenada
        for (int j = i + 1; j < n; ++j) {
            int current_priority = fila[j].sintomas; // Obtém a prioridade do paciente atual

            // Se o paciente atual tem uma prioridade maior, atualiza o índice máximo
            if (current_priority > max_priority) {
                max_idx = j;
                max_priority = current_priority;
            }
        }

        // Se o índice máximo mudou, troca os pacientes para ordenar
        if (max_idx != i) {
            std::swap(fila[i], fila[max_idx]);
        }
    }
}

/**
 * @brief Ordena a fila de pacientes com base no nome usando o algoritmo de seleção.
 * Não possui parâmetros de entrada ou retorno.
 */
void ordenar_paciente_nome() {
    sorted_name_fila = fila;               // Cria uma cópia da fila original para ordenar
    int n = sorted_name_fila.size();      // Obtém o tamanho da fila ordenada

    // Loop externo para cada posição na fila ordenada
    for (int i = 0; i < n - 1; ++i) {
        int min_idx = i; // Assume que o primeiro elemento é o mínimo

        // Loop interno para encontrar o paciente com o menor nome na parte não ordenada
        for (int j = i + 1; j < n; ++j) {
            // Compara os nomes lexicograficamente de forma insensível a maiúsculas/minúsculas
            if (sorted_name_fila[j].name.compare(sorted_name_fila[min_idx].name, Qt::CaseInsensitive) < 0) {
                min_idx = j; // Atualiza o índice mínimo se um nome menor for encontrado
            }
        }

        // Se o índice mínimo mudou, troca os pacientes para ordenar
        if (min_idx != i) {
            std::swap(sorted_name_fila[i], sorted_name_fila[min_idx]);
        }
    }
}

// Destrutor da classe MainWindow
MainWindow::~MainWindow()
{
    delete ui; // Libera a memória alocada para a interface do usuário
}

/**
 * @brief Função chamada quando o botão de adicionar/atualizar paciente é clicado.
 * Captura os dados do paciente, valida e adiciona ou atualiza na fila.
 * Não possui parâmetros de entrada ou retorno.
 */
void MainWindow::on_patientButton_clicked()
{
    bool paciente_cadastrado = false;       // Flag para verificar se o paciente já está cadastrado
    Patient * paciente_existente = nullptr;  // Ponteiro para o paciente existente
    unsigned int sintomas = 0;               // Variável para armazenar os sintomas selecionados
    QString name = ui->nameLineEdit->text(); // Obtém o nome inserido no campo de texto
    QString cpf = ui->cpfLineEdit->text();   // Obtém o CPF inserido no campo de texto
    QString email = ui->emailLineEdit->text(); // Obtém o email inserido no campo de texto
    QDate bday = ui->birthdayDateEdit->date();  // Obtém a data de nascimento inserida

    // Verifica se o campo CPF está vazio
    if (cpf.isEmpty()) {
        QMessageBox::warning(this, "Erro de Entrada", "Por favor, insira um CPF.");
        return;
    }

    // Verifica se o campo Nome está vazio
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Erro de Entrada", "Por favor, insira um nome.");
        return;
    }

    // Valida o formato do CPF (XXX.XXX.XXX-XX)
    if(cpf[3] != '.' || cpf[7] != '.' || cpf[11] != '-'){
        ui->cpfLineEdit->setText(""); // Limpa o campo CPF
        QMessageBox::warning(this, "Erro de Entrada", "O CPF deve seguir o formato XXX.XXX.XXX-XX");
        return;
    }

    // Verifica quais sintomas foram selecionados e atualiza a variável 'sintomas' usando operações bitwise
    if(ui->sintoma_tosseCheckBox->isChecked()){
        sintomas |= TOSSE;
    }
    if(ui->sintoma_nauseaCheckBox->isChecked()){
        sintomas |= NAUSEA;
    }
    if(ui->sintoma_febreCheckBox->isChecked()){
        sintomas |= FEBRE;
    }
    if(ui->sintoma_covidCheckBox->isChecked()){
        sintomas |= COVID;
    }
    if(ui->sintoma_acidenteCheckBox->isChecked()){
        sintomas |= ACIDENTE;
    }
    if(ui->sintoma_avcCheckBox->isChecked()){
        sintomas |= AVC;
    }
    if(ui->sintoma_infeccaoCheckBox->isChecked()){
        sintomas |= INFECCAO;
    }
    if(ui->sintoma_disenteriaCheckBox->isChecked()){
        sintomas |= DISENTERIA;
    }

    // Se nenhum sintoma foi selecionado, encerra a função
    if(sintomas == 0) {
        QMessageBox::warning(this, "Erro de Entrada", "Por favor, selecione pelo menos um sintoma.");
        return;
    }

    // Percorre a fila para verificar se o paciente já está cadastrado
    for (Patient& paciente : fila) {
        if(paciente.cpf.compare(cpf) == 0){
            paciente_cadastrado = true;     // Marca que o paciente está cadastrado
            paciente_existente = &paciente; // Aponta para o paciente existente
            break;                           // Sai do loop após encontrar o paciente
        }
    }

    if(paciente_cadastrado){
        // Atualiza os dados do paciente existente
        QDate current_date = QDate::currentDate(); // Obtém a data atual
        paciente_existente->name = name;              // Atualiza o nome
        paciente_existente->email = email;            // Atualiza o email
        paciente_existente->bday = bday;              // Atualiza a data de nascimento
        paciente_existente->sintomas = sintomas;      // Atualiza os sintomas
        paciente_existente->idade = current_date.year() - bday.year(); // Recalcula a idade

        // Ajusta a idade se o aniversário ainda não ocorreu no ano atual
        if (current_date.month() < bday.month() ||
            (current_date.month() == bday.month() && current_date.day() < bday.day())) {
            paciente_existente->idade--;
        }
    }
    else{
        // Cria um novo paciente e adiciona à fila e ao relatório
        Patient new_patient = Patient(name, cpf, email, bday, sintomas);
        fila.push_back(new_patient);           // Adiciona o paciente à fila
        ordenar_paciente_sintoma();           // Ordena a fila por sintomas
        relatorio.push_back(new_patient);      // Adiciona o paciente ao relatório de tratados
    }

    // Limpa os campos de entrada na interface após adicionar/atualizar o paciente
    ui->nameLineEdit->setText("");
    ui->cpfLineEdit->setText("");
    ui->emailLineEdit->setText("");

    // Desmarca todos os checkboxes de sintomas
    ui->sintoma_tosseCheckBox->setCheckState(Qt::CheckState::Unchecked);
    ui->sintoma_nauseaCheckBox->setCheckState(Qt::CheckState::Unchecked);
    ui->sintoma_febreCheckBox->setCheckState(Qt::CheckState::Unchecked);
    ui->sintoma_covidCheckBox->setCheckState(Qt::CheckState::Unchecked);
    ui->sintoma_acidenteCheckBox->setCheckState(Qt::CheckState::Unchecked);
    ui->sintoma_avcCheckBox->setCheckState(Qt::CheckState::Unchecked);
    ui->sintoma_infeccaoCheckBox->setCheckState(Qt::CheckState::Unchecked);
    ui->sintoma_disenteriaCheckBox->setCheckState(Qt::CheckState::Unchecked);
}

/**
 * @brief Realiza uma busca binária pelo nome do paciente.
 * @param name Nome do paciente a ser buscado.
 * @return Índice do paciente encontrado ou -1 se não encontrado.
 */
int binary_search(const QString& name) {
    int left = 0;                             // Início do intervalo de busca
    int right = sorted_name_fila.size() - 1; // Fim do intervalo de busca

    // Enquanto o intervalo de busca for válido
    while (left <= right) {
        int mid = left + (right - left) / 2; // Calcula o índice do meio
        // Compara o nome alvo com o nome do paciente no índice do meio
        int cmp = sorted_name_fila[mid].name.compare(name, Qt::CaseInsensitive);

        if (cmp == 0) {
            return mid; // Paciente encontrado, retorna o índice
        }
        else if (cmp < 0) {
            left = mid + 1; // O nome alvo está na metade direita
        }
        else {
            right = mid - 1; // O nome alvo está na metade esquerda
        }
    }
    return -1; // Paciente não encontrado
}

/**
 * @brief Função chamada quando o botão de consultar paciente é clicado.
 * Realiza a busca do paciente pelo CPF ou nome e exibe as informações.
 * Não possui parâmetros de entrada ou retorno.
 */
void MainWindow::on_consultarPatient_clicked()
{
    ui->textBrowser->clear();
    ui->textBrowser->append("Início de listagem");
    QString cpf = ui->cpfLineEdit->text();   // Obtém o CPF inserido
    QString name = ui->nameLineEdit->text(); // Obtém o nome inserido

    // Verifica se a busca binária está selecionada
    if(ui->buscaBinariaCheckbox->isChecked()){
        ordenar_paciente_nome(); // Ordena os registros da fila em sorted_name_fila
        int idx = binary_search(name); // Realiza a busca binária pelo nome

        // Verifica se o paciente foi encontrado
        if(idx != -1){
            Patient patient = sorted_name_fila.at(idx); // Obtém o paciente encontrado
            QString info;
            info += "ID: " + QString::number(patient.id) + "\n";                        // Adiciona o ID
            info += "Nome: " + patient.name + "\n";                                      // Adiciona o nome
            info += "CPF: " + patient.cpf + "\n";                                        // Adiciona o CPF
            info += "Email: " + patient.email + "\n";                                    // Adiciona o email
            info += "Data de Nascimento: " + patient.bday.toString("dd/MM/yyyy") + "\n"; // Adiciona a data de nascimento
            info += "Idade: " + QString::number(patient.idade) + "\n";                   // Adiciona a idade
            info += "Sintomas: ";                                                        // Início da lista de sintomas

            // Verifica e adiciona os sintomas do paciente
            if (patient.sintomas & TOSSE)       info += "TOSSE ";
            if (patient.sintomas & NAUSEA)      info += "NÁUSEA ";
            if (patient.sintomas & FEBRE)       info += "FEBRE ";
            if (patient.sintomas & DISENTERIA)  info += "DISENTERIA ";
            if (patient.sintomas & INFECCAO)    info += "INFECÇÃO ";
            if (patient.sintomas & COVID)       info += "COVID ";
            if (patient.sintomas & AVC)         info += "AVC ";
            if (patient.sintomas & ACIDENTE)    info += "ACIDENTE ";

            info += "\n-----------------------------\n"; // Adiciona uma linha separadora

            // Adiciona as informações ao TextBrowser na interface
            ui->textBrowser->append(info);
            return; // Sai da função após exibir as informações
        }

        // Se o paciente não foi encontrado, exibe uma mensagem de aviso
        QMessageBox::warning(this, "Não Encontrado", "Nenhum paciente com o nome \"" + name + "\" foi encontrado.");
        return;
    }

    // Se a busca binária não estiver selecionada, realiza uma busca sequencial
    for (Patient& patient : fila) {
        // Verifica se o CPF ou o nome do paciente correspondem aos critérios de busca
        if( (!cpf.isEmpty() && patient.cpf.compare(cpf) == 0) ||
            (!name.isEmpty() && patient.name.toLower().compare(name.toLower()) == 0)
            ){
            QString info;
            info += "ID: " + QString::number(patient.id) + "\n";                        // Adiciona o ID
            info += "Nome: " + patient.name + "\n";                                      // Adiciona o nome
            info += "CPF: " + patient.cpf + "\n";                                        // Adiciona o CPF
            info += "Email: " + patient.email + "\n";                                    // Adiciona o email
            info += "Data de Nascimento: " + patient.bday.toString("dd/MM/yyyy") + "\n"; // Adiciona a data de nascimento
            info += "Idade: " + QString::number(patient.idade) + "\n";                   // Adiciona a idade
            info += "Sintomas: ";                                                        // Início da lista de sintomas

            // Verifica e adiciona os sintomas do paciente
            if (patient.sintomas & TOSSE)       info += "TOSSE ";
            if (patient.sintomas & NAUSEA)      info += "NÁUSEA ";
            if (patient.sintomas & FEBRE)       info += "FEBRE ";
            if (patient.sintomas & DISENTERIA)  info += "DISENTERIA ";
            if (patient.sintomas & INFECCAO)    info += "INFECÇÃO ";
            if (patient.sintomas & COVID)       info += "COVID ";
            if (patient.sintomas & AVC)         info += "AVC ";
            if (patient.sintomas & ACIDENTE)    info += "ACIDENTE ";

            info += "\n-----------------------------\n"; // Adiciona uma linha separadora

            // Adiciona as informações ao TextBrowser na interface
            ui->textBrowser->append(info);
            return; // Sai da função após encontrar e exibir o paciente
        }
    }

    // Se o paciente não foi encontrado, exibe uma mensagem de aviso
    QMessageBox::warning(this, "Não Encontrado", "Paciente não encontrado.");
}

/**
 * @brief Função chamada quando a edição do campo CPF é finalizada para validar o formato.
 * Não possui parâmetros de entrada ou retorno.
 */
void MainWindow::on_cpfLineEdit_editingFinished()
{
    QString cpf = ui->cpfLineEdit->text(); // Obtém o CPF inserido
    // Verifica se o CPF segue o formato XXX.XXX.XXX-XX
    if(cpf[3] != '.' || cpf[7] != '.' || cpf[11] != '-'){
        ui->cpfLineEdit->setText(""); // Limpa o campo CPF
        QMessageBox::warning(this, "Erro de Entrada", "O CPF deve seguir o formato XXX.XXX.XXX-XX"); // Exibe mensagem de erro
        return;
    }
}

/**
 * @brief Função chamada quando o botão para gerar relatório é clicado.
 * Não possui parâmetros de entrada ou retorno.
 */
void MainWindow::on_pushButton_clicked()
{
    ui->textBrowser->clear();
    gerar_relatorio(); // Chama a função para gerar o relatório dos pacientes tratados
}

/**
 * @brief Função chamada quando o botão para remover um paciente é clicado.
 * Remove o paciente da fila com base no CPF inserido.
 * Não possui parâmetros de entrada ou retorno.
 */
void MainWindow::on_removePatient_clicked()
{
    QString cpf = ui->cpfLineEdit->text(); // Obtém o CPF inserido

    // Verifica se o campo CPF está vazio
    if (cpf.isEmpty()) {
        QMessageBox::warning(this, "Erro de Entrada", "Por favor, insira um CPF."); // Exibe mensagem de aviso
        return;
    }

    // Percorre a fila para encontrar o paciente com o CPF correspondente
    for (int i = 0; i < fila.size(); ++i) {
        if(fila[i].cpf.compare(cpf) == 0){
            fila.erase(fila.begin()+i); // Remove o paciente da fila
            QMessageBox::information(this, "Paciente Removido", "Paciente removido com sucesso.");
            return; // Sai do loop após remover o paciente
        }
    }

    // Se o paciente não foi encontrado, exibe uma mensagem de aviso
    QMessageBox::warning(this, "Não Encontrado", "Paciente não encontrado na fila.");
}

/**
 * @brief Função chamada quando o botão para listar pacientes na fila é clicado.
 * Exibe todos os pacientes atualmente na fila.
 * Não possui parâmetros de entrada ou retorno.
 */
void MainWindow::on_pushButton_2_clicked()
{
    ui->textBrowser->clear();
    ui->textBrowser->append("Início de listagem");
    for (Patient& patient : fila) {
        QString info;
        info += "ID: " + QString::number(patient.id) + "\n";                        // Adiciona o ID
        info += "Nome: " + patient.name + "\n";                                      // Adiciona o nome
        info += "CPF: " + patient.cpf + "\n";                                        // Adiciona o CPF
        info += "Email: " + patient.email + "\n";                                    // Adiciona o email
        info += "Data de Nascimento: " + patient.bday.toString("dd/MM/yyyy") + "\n"; // Adiciona a data de nascimento
        info += "Idade: " + QString::number(patient.idade) + "\n";                   // Adiciona a idade
        info += "Sintomas: ";                                                        // Início da lista de sintomas

        // Verifica e adiciona os sintomas do paciente
        if (patient.sintomas & TOSSE)       info += "TOSSE ";
        if (patient.sintomas & NAUSEA)      info += "NÁUSEA ";
        if (patient.sintomas & FEBRE)       info += "FEBRE ";
        if (patient.sintomas & DISENTERIA)  info += "DISENTERIA ";
        if (patient.sintomas & INFECCAO)    info += "INFECÇÃO ";
        if (patient.sintomas & COVID)       info += "COVID ";
        if (patient.sintomas & AVC)         info += "AVC ";
        if (patient.sintomas & ACIDENTE)    info += "ACIDENTE ";

        info += "\n-----------------------------\n"; // Adiciona uma linha separadora

        // Adiciona as informações ao TextBrowser na interface
        ui->textBrowser->append(info);
    }
}
