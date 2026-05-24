#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QClipboard>
#include <QApplication>
#include <QMenu>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. 初始化数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sql_info.db");

    if (db.open()) {
        QSqlQuery query;
        // 强制删除旧表，确保新数据成功写入
        query.exec("DROP TABLE IF EXISTS sql_statements");

        query.exec("CREATE TABLE IF NOT EXISTS sql_statements ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, category TEXT, name TEXT, "
                   "desc_zh TEXT, desc_en TEXT, code_zh TEXT, code_en TEXT);");

        // 批量插入数据
        QList<QStringList> initialData = {
            {"0. 基础环境与会话管理", "CONNECT (普通切管理)", "从普通员工只读账号切换到HR管理员账号。", "Switch from a regular read-only employee account to the HR admin account.", "CONNECT hr_admin/password123;", "CONNECT hr_admin/password123;"},
            {"0. 基础环境与会话管理", "CONNECT (最高权限)", "系统崩溃，切换到数据库最高权限账号进行紧急抢修。", "The system crashed. Switch to the highest privilege account for emergency repair.", "CONNECT sys AS sysdba;", "CONNECT sys AS sysdba;"},
            {"0. 基础环境与会话管理", "CONNECT (特定业务库)", "进入月底结账期，切换到财务专属数据库实例。", "Enter the end-of-month closing period and switch to the finance database instance.", "CONNECT finance_user/fin_pwd@finance_db;", "CONNECT finance_user/fin_pwd@finance_db;"},
            {"0. 基础环境与会话管理", "SHOW DATABASES", "DBA接手新服务器，需要确认上面部署了哪些子公司的数据库。", "The DBA takes over a new server and needs to confirm which subsidiary databases are deployed.", "SHOW DATABASES;", "SHOW DATABASES;"},
            {"0. 基础环境与会话管理", "SELECT DATABASE()", "在执行大批量删改前，确认自己是不是在测试库，防止删库跑路。", "Confirm whether you are in the test database before executing bulk deletions or updates.", "SELECT DATABASE();", "SELECT DATABASE();"},
            {"0. 基础环境与会话管理", "CREATE DATABASE", "集团成立了全新的海外事业部，为其建立独立的数据库环境。", "The group established a new overseas business division and built an independent database environment.", "CREATE DATABASE overseas_branch_db;", "CREATE DATABASE overseas_branch_db;"},
            {"0. 基础环境与会话管理", "@执行脚本", "新系统上线，一键执行包含了建表和基础数据的初始化大脚本。", "Go live with the new system. Execute the large initialization script containing tables and base data with one click.", "@/usr/local/scripts/init_company_data.sql;", "@/usr/local/scripts/init_company_data.sql;"},
            {"0. 基础环境与会话管理", "PURGE RECYCLEBIN", "彻底清除被 DROP 掉的离职员工备份表，释放宝贵的服务器磁盘空间。", "Completely clear the dropped backup tables of resigned employees to free up server disk space.", "PURGE RECYCLEBIN;", "PURGE RECYCLEBIN;"},
            {"0. 基础环境与会话管理", "-- / /* */ 注释", "在复杂的发薪水脚本中写明逻辑。单行使用--，多行使用/**/。", "Document business logic in complex salary scripts using single-line or multi-line comments.", "-- 发放基本工资\n/* 扣除五险一金和迟到罚款 */", "-- Basic Salary\n/* Deduct social security and fines */"},
            {"1. DDL (数据定义语言)", "CREATE TABLE (Varchar)", "创建员工表，考虑海外号码，使用变长字符串。", "Create an employee table using variable-length strings considering overseas phone numbers.", "CREATE TABLE employees (emp_id INT, phone_number VARCHAR(20));", "CREATE TABLE employees (emp_id INT, phone_number VARCHAR(20));"},
            {"1. DDL (数据定义语言)", "COMMENT 注释", "创建部门表时，加上注释说明，防止后来的程序员看不懂。", "Add table comments when creating the department table to prevent future confusion.", "CREATE TABLE departments (dept_id INT PRIMARY KEY) COMMENT='集团核心部门表';", "CREATE TABLE departments (dept_id INT PRIMARY KEY) COMMENT='Group Core Dept Table';"},
            {"1. DDL (数据定义语言)", "CREATE TABLE (绩效表)", "创建绩效表，记录每年的考核分数。", "Create a performance table to record annual evaluation scores.", "CREATE TABLE performance (emp_id INT, score INT, year INT);", "CREATE TABLE performance (emp_id INT, score INT, year INT);"},
            {"1. DDL (数据定义语言)", "SHOW TABLES", "财务部想知道当前数据库里总共有几张表。", "The finance department wants to know the total number of tables in the current database.", "SHOW TABLES;", "SHOW TABLES;"},
            {"1. DDL (数据定义语言)", "DESC 架构查看", "HR 想查一下 employees 表里有没有“身份证号”这个字段。", "HR wants to check if the 'ID number' field exists in the employees table.", "DESC employees;", "DESC employees;"},
            {"1. DDL (数据定义语言)", "SHOW CREATE TABLE", "需要复刻一张与老系统完全一致的薪资表，查看当初建表时的原始代码。", "View the original definition statement to replicate a salary table identical to the old system.", "SHOW CREATE TABLE old_salary_table;", "SHOW CREATE TABLE old_salary_table;"},
            {"1. DDL (数据定义语言)", "RENAME TABLE", "公司组织架构大调，原“分公司表”改名为“大区表”。", "Company organizational restructure: rename the original branch table to regional table.", "RENAME TABLE branch_offices TO regional_offices;", "RENAME TABLE branch_offices TO regional_offices;"},
            {"1. DDL (数据定义语言)", "ALTER RENAME (归档)", "将去年的打卡记录表重命名为归档表。", "Rename last year's attendance log table to an archive table.", "ALTER TABLE attendance RENAME TO attendance_2023;", "ALTER TABLE attendance RENAME TO attendance_2023;"},
            {"1. DDL (数据定义语言)", "ALTER RENAME (备份)", "大促前，备份当前的产品价格表。", "Backup the current product price table before the grand promotion.", "ALTER TABLE product_price RENAME TO product_price_bak;", "ALTER TABLE product_price RENAME TO product_price_bak;"},
            {"1. DDL (数据定义语言)", "ALTER ADD (增加字段)", "疫情期间，HR 要求在员工系统里新增“紧急联系人”列。", "HR requires adding an 'emergency contact' column to the employee system during the pandemic.", "ALTER TABLE employees ADD emergency_contact VARCHAR(50);", "ALTER TABLE employees ADD emergency_contact VARCHAR(50);"},
            {"1. DDL (数据定义语言)", "ALTER MODIFY (修改长度)", "招了俄罗斯外籍员工，名字太长存不下，扩充姓名列的容量。", "Expand the capacity of the name column to accommodate long foreign names.", "ALTER TABLE employees MODIFY name VARCHAR(255);", "ALTER TABLE employees MODIFY name VARCHAR(255);"},
            {"1. DDL (数据定义语言)", "ALTER DROP (删除字段)", "为规避隐私风险，彻底删除员工表中的“家庭具体门牌号”列。", "Completely drop the home address column from the employee table to avoid privacy risks.", "ALTER TABLE employees DROP COLUMN home_address;", "ALTER TABLE employees DROP COLUMN home_address;"},
            {"1. DDL (数据定义语言)", "ALTER ADD CONSTRAINT (外键)", "规范数据，确保员工表里的部门 ID 必须真实存在于部门表中。", "Standardize data to ensure that the dept_id in the employee table exists in the department table.", "ALTER TABLE employees ADD CONSTRAINT fk_dept FOREIGN KEY (dept_id) REFERENCES departments(id);", "ALTER TABLE employees ADD CONSTRAINT fk_dept FOREIGN KEY (dept_id) REFERENCES departments(id);"},
            {"1. DDL (数据定义语言)", "ON DELETE CASCADE (级联)", "设定当某项目组被整体裁撤时，挂靠在该项目下的临时岗位自动销毁。", "Set temporary positions to be automatically destroyed when their project group is dissolved.", "ALTER TABLE temp_positions ADD CONSTRAINT fk_proj FOREIGN KEY (project_id) REFERENCES projects(id) ON DELETE CASCADE;", "ALTER TABLE temp_positions ADD CONSTRAINT fk_proj FOREIGN KEY (project_id) REFERENCES projects(id) ON DELETE CASCADE;"},
            {"1. DDL (数据定义语言)", "ALTER DROP CONSTRAINT", "公司搬迁后车位充裕，取消了原表里“每个员工车牌号必须唯一”的硬性约束。", "Drop the unique constraint on license plates as parking spaces became abundant.", "ALTER TABLE employees DROP CONSTRAINT unique_plate_number;", "ALTER TABLE employees DROP CONSTRAINT unique_plate_number;"},
            {"1. DDL (数据定义语言)", "DROP TABLE", "2015 年的作废福利表已经过了审计期，连表带数据彻底销毁。", "The obsolete 2015 welfare table has passed its audit period. Destroy it completely.", "DROP TABLE welfare_2015;", "DROP TABLE welfare_2015;"},
            {"1. DDL (数据定义语言)", "TRUNCATE TABLE (清空)", "每月初，一键清空上个月的打卡临时计算表，只保留表头给这个月用。", "Clear last month's temporary attendance calculation table while keeping the structure.", "TRUNCATE TABLE monthly_punch_temp;", "TRUNCATE TABLE monthly_punch_temp;"},
            {"1. DDL (数据定义语言)", "TRUNCATE TABLE (清理环境)", "新系统正式上线前夜，用最快的速度把 HR 制造的所有假员工测试数据瞬间清空。", "Instantly clear all dummy employee test data created by HR the night before going live.", "TRUNCATE TABLE employees_test;", "TRUNCATE TABLE employees_test;"},
            {"2. DML (数据操作语言)", "INSERT INTO (单条)", "行政部录入今天刚刚办理完入职手续的保洁王阿姨。", "The administration department registers Aunt Wang who just finished onboarding today.", "INSERT INTO employees (id, name, position) VALUES (998, '王阿姨', '保洁');", "INSERT INTO employees (id, name, position) VALUES (998, 'Aunt Wang', 'Janitor');"},
            {"2. DML (数据操作语言)", "INSERT ALL (批量)", "校招结束，一次性将 3 位应届生录入系统。", "Campus recruitment ends. Insert 3 fresh graduates into the system at once.", "INSERT ALL INTO employees VALUES (101, '张三') INTO employees VALUES (102, '李四') INTO employees VALUES (103, '王五') SELECT * FROM dual;", "INSERT ALL INTO employees VALUES (101, 'Zhang San') INTO employees VALUES (102, 'Li Si') INTO employees VALUES (103, 'Wang Wu') SELECT * FROM dual;"},
            {"2. DML (数据操作语言)", "INSERT (指定列)", "只插入员工姓名和工号，因为新员工暂时还没分配到具体的工位和部门。", "Insert only the employee name and ID because no station or department is assigned yet.", "INSERT INTO employees (emp_id, name) VALUES (104, '赵六');", "INSERT INTO employees (emp_id, name) VALUES (104, 'Zhao Liu');"},
            {"2. DML (数据操作语言)", "UPDATE (条件修改)", "年度普调，将工号为 001 的老员工底薪增加 2000 元。", "Annual adjustment: increase the base salary of senior employee '001' by 2000 RMB.", "UPDATE salary_info SET base_salary = base_salary + 2000 WHERE emp_id = '001';", "UPDATE salary_info SET base_salary = base_salary + 2000 WHERE emp_id = '001';"},
            {"2. DML (数据操作语言)", "UPDATE (批量修改)", "公司架构合并，将所有原属于“市场二部”的人员全部划归到“市场一部”。", "Company merger: transfer all personnel from Marketing Dept 2 to Marketing Dept 1.", "UPDATE employees SET dept_id = 'M01' WHERE dept_id = 'M02';", "UPDATE employees SET dept_id = 'M01' WHERE dept_id = 'M02';"},
            {"2. DML (数据操作语言)", "UPDATE (REPLACE 清洗)", "历史遗留数据清洗，将系统生成带有特殊美元符号的临时账单号替换为常规前缀。", "Data cleansing: replace temporary bill numbers containing a '$' symbol with a standard prefix.", "UPDATE orders SET bill_no = REPLACE(bill_no, '$', 'BILL-');", "UPDATE orders SET bill_no = REPLACE(bill_no, '$', 'BILL-');"},
            {"2. DML (数据操作语言)", "DELETE (条件删除)", "删除因试用期表现极差而被辞退的员工记录。", "Delete the record of an employee dismissed due to extremely poor performance during probation.", "DELETE FROM employees WHERE status = '试用期不合格';", "DELETE FROM employees WHERE status = 'Failed Probation';"},
            {"2. DML (数据操作语言)", "DELETE (全表清空)", "由于录入严重失误，财务部需要撤回今天的全部手工发薪流水，准备重做。", "Roll back today's entire manual payroll stream due to critical entry errors.", "DELETE FROM manual_salary_flow;", "DELETE FROM manual_salary_flow;"},
            {"2. DML (数据操作语言)", "DELETE (范围删除)", "系统年末大扫除，删除所有年龄大于 60 岁且状态已经是“已退休”的老旧合同记录。", "Year-end system cleanup: delete old contract records where age > 60 and status is 'Retired'.", "DELETE FROM contracts WHERE age > 60 AND status = 'Retired';", "DELETE FROM contracts WHERE age > 60 AND status = 'Retired';"},
            {"3. DQL (数据查询与分析)", "SELECT AS (别名)", "为了给不懂英文的老板看报表，将列名翻译为中文显示。", "Translate column headers into Chinese aliases for presentations to the executive board.", "SELECT name AS 员工姓名, phone_number AS 手机号码 FROM employees;", "SELECT name AS 员工姓名, phone_number AS 手机号码 FROM employees;"},
            {"3. DQL (数据查询与分析)", "表别名", "在写几百行的长语句时为了少打字，给员工表起个简写。", "Use short table aliases to reduce keystrokes when writing massive query statements.", "SELECT e.name, e.salary FROM employees e;", "SELECT e.name, e.salary FROM employees e;"},
            {"3. DQL (数据查询与分析)", "UPPER 函数", "在系统中搜索外企客户名时，忽略大小写，将数据库里的名字全转成大写来比对。", "Ignore case sensitivity by converting database client names to uppercase for comparison.", "SELECT * FROM clients WHERE UPPER(english_name) = 'APPLE INC';", "SELECT * FROM clients WHERE UPPER(english_name) = 'APPLE INC';"},
            {"3. DQL (数据查询与分析)", "ORDER BY DESC (降序)", "发年终奖时，HR 需要一份按考核分数从高到低排列的“功臣名单”。", "HR needs a merit list sorted by evaluation scores from highest to lowest for year-end bonuses.", "SELECT name, score FROM performance ORDER BY score DESC;", "SELECT name, score FROM performance ORDER BY score DESC;"},
            {"3. DQL (数据查询与分析)", "ORDER BY ASC (升序)", "行政部准备颁发“五年陈”纪念戒指，需要按照入职时间从早到晚排列员工。", "Sort employees by hire date from earliest to latest for the 5-year loyalty ring award.", "SELECT name, hire_date FROM employees ORDER BY hire_date ASC;", "SELECT name, hire_date FROM employees ORDER BY hire_date ASC;"},
            {"3. DQL (数据查询与分析)", "多级复合排序", "打绩效时，先按“部门编号”排到一起，同一个部门里再按“薪资水平”从高到低排。", "Sort multi-level index: first group by department ID, then sort by salary descending.", "SELECT dept_id, name, salary FROM employees ORDER BY dept_id ASC, salary DESC;", "SELECT dept_id, name, salary FROM employees ORDER BY dept_id ASC, salary DESC;"},
            {"3. DQL (数据查询与分析)", "COUNT 聚合", "统计今天考勤机里总共有多少条打卡流水，以及实际来上班的独立人数。", "Count total attendance logs and distinct individuals who clocked in today.", "SELECT Count(*) FROM attendance;\nSELECT Count(distinct emp_id) FROM attendance;", "SELECT Count(*) FROM attendance;\nSELECT Count(distinct emp_id) FROM attendance;"},
            {"3. DQL (数据查询与分析)", "SUM / AVG 聚合", "财务总监想知道本月“研发部”总共发了多少总薪水，以及这个部门的平均薪水。", "Calculate total and average payroll expenditures for the R&D department this month.", "SELECT SUM(salary), AVG(salary) FROM employees WHERE dept_name = '研发部';", "SELECT SUM(salary), AVG(salary) FROM employees WHERE dept_name = '研发部';"},
            {"3. DQL (数据查询与分析)", "GROUP BY 结合", "查询每个部门的最高和最低工资，用来分析各部门内部的贫富差距情况。", "Query max and min salaries per department to analyze internal income distribution.", "SELECT dept_id, MAX(salary) AS 最高薪, MIN(salary) AS 最低薪 FROM employees GROUP BY dept_id;", "SELECT dept_id, MAX(salary) AS MaxSalary, MIN(salary) AS MinSalary FROM employees GROUP BY dept_id;"},
            {"3. DQL (数据查询与分析)", "笛卡尔积灾难 (避免)", "新手实习生想查员工的部门忘了写条件相乘。必须通过外键相等来建立连接！", "Avoid cross-product disasters. Build a join bridge explicitly via matching foreign keys.", "SELECT * FROM employees e, departments d WHERE e.dept_id = d.id;", "SELECT * FROM employees e, departments d WHERE e.dept_id = d.id;"},
            {"3. DQL (数据查询与分析)", "CROSS JOIN (配对)", "公司年会要搞抽奖匹配游戏，需要把所有的男员工和女员工做全排列配对。", "Purposefully execute a cross join to pair male and female staff for annual gala games.", "SELECT m.name, f.name FROM male_staff m CROSS JOIN female_staff f;", "SELECT m.name, f.name FROM male_staff m CROSS JOIN female_staff f;"},
            {"3. DQL (数据查询与分析)", "INNER JOIN (内连接)", "HR 要打印花名册，但只打印那些“已经正式分配了具体部门”的员工。", "Print employee roster excluding unassigned onboarding freshers.", "SELECT e.name, d.dept_name FROM employees e INNER JOIN departments d ON e.dept_id = d.id;", "SELECT e.name, d.dept_name FROM employees e INNER JOIN departments d ON e.dept_id = d.id;"},
            {"3. DQL (数据查询与分析)", "三表连查", "财务核算年终奖，需要同时连结员工表、部门表、绩效表。", "Join three tables simultaneously (Employees, Departments, Performance) for bonuses.", "SELECT e.name, d.cost_center, p.multiplier FROM employees e INNER JOIN departments d ON e.dept_id=d.id INNER JOIN performance p ON e.id=p.emp_id;", "SELECT e.name, d.cost_center, p.multiplier FROM employees e INNER JOIN departments d ON e.dept_id=d.id INNER JOIN performance p ON e.id=p.emp_id;"},
            {"3. DQL (数据查询与分析)", "LEFT JOIN (左外连)", "拉全公司大名单，即使有些刚报到的应届生还没有部门，也要显示名字。", "Fetch entire company roster, ensuring freshers without department fields are included.", "SELECT e.name, d.dept_name FROM employees e LEFT JOIN departments d ON e.dept_id = d.id;", "SELECT e.name, d.dept_name FROM employees e LEFT JOIN departments d ON e.dept_id = d.id;"},
            {"3. DQL (数据查询与分析)", "LEFT JOIN (查空)", "找出所有“有名字，但没有今年考核成绩”的员工，准备催促他们交报告。", "Identify employees who have profiles but lack evaluation scores to push reminders.", "SELECT e.name FROM employees e LEFT JOIN performance p ON e.id = p.emp_id WHERE p.score IS NULL;", "SELECT e.name FROM employees e LEFT JOIN performance p ON e.id = p.emp_id WHERE p.score IS NULL;"},
            {"4. DCL (数据控制语言)", "GRANT SELECT (只读)", "赋予新来的部门经理查看自己部门员工薪水的权限，但绝不能让他修改。", "Grant read-only salary table view access to the newly appointed manager.", "GRANT SELECT ON salary_table TO 'manager_role';", "GRANT SELECT ON salary_table TO 'manager_role';"},
            {"4. DCL (数据控制语言)", "GRANT INSERT", "允许 HR 专员向员工表里录入新人。", "Grant insert privileges to HR staff to registry newcomers.", "GRANT INSERT ON employees TO 'hr_staff';", "GRANT INSERT ON employees TO 'hr_staff';"},
            {"4. DCL (数据控制语言)", "REVOKE (权限回收)", "某经理被降职，立刻收回他偷看全公司薪资表的权限。", "Revoke salary table selection privileges immediately due to manager demotion.", "REVOKE SELECT ON salary_table FROM 'demoted_manager';", "REVOKE SELECT ON salary_table FROM 'demoted_manager';"},
            {"5. TCL (事务控制语言)", "COMMIT / ROLLBACK", "财务发工资银行跨表转账，如果中途断电必须 ROLLBACK，全顺利则 COMMIT。", "Bank transfer transactions: ROLLBACK if power cuts mid-way, COMMIT if successful.", "COMMIT;\nROLLBACK;", "COMMIT;\nROLLBACK;"},
            {"5. TCL (事务控制语言)", "SAVEPOINT (保存点)", "执行年底极度复杂的跨部门薪资结转，执行完第一部门后设保存点。", "Set a restore boundary after calculating the first department's year-end payroll.", "SAVEPOINT after_dept_1;", "SAVEPOINT after_dept_1;"},
            {"5. TCL (事务控制语言)", "ROLLBACK TO", "在结转第二部门时出现严重报错，只需撤销到存档点，无需推翻第一部门。", "Roll back partial steps to savepoint when Department 2 computation faults.", "ROLLBACK TO SAVEPOINT after_dept_1;", "ROLLBACK TO SAVEPOINT after_dept_1;"}
        };

        for (const QStringList &row : initialData) {
            QSqlQuery ins;
            ins.prepare("INSERT INTO sql_statements (category, name, desc_zh, desc_en, code_zh, code_en) VALUES (?,?,?,?,?,?)");
            ins.addBindValue(row[0]); ins.addBindValue(row[1]); ins.addBindValue(row[2]);
            ins.addBindValue(row[3]); ins.addBindValue(row[4]); ins.addBindValue(row[5]);
            ins.exec();
        }
    }

    // 2. UI 控件初始化
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("搜索 SQL 语句...");
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    languageButton = new QPushButton("Language", this);
    QMenu *langMenu = new QMenu(this);
    QAction *actZh = langMenu->addAction("中文");
    QAction *actEn = langMenu->addAction("English");
    languageButton->setMenu(langMenu);

    connect(actZh, &QAction::triggered, this, [this]() { currentLang = "中文"; updateLanguage(); });
    connect(actEn, &QAction::triggered, this, [this]() { currentLang = "English"; updateLanguage(); });

    sqlTree = new QTreeWidget(this);
    sqlTree->setHeaderHidden(true);
    detailBrowser = new QTextBrowser(this);
    copyButton = new QPushButton("一键复制", this);

    connect(sqlTree, &QTreeWidget::itemClicked, this, &MainWindow::onTreeItemClicked);
    connect(copyButton, &QPushButton::clicked, this, &MainWindow::onCopyButtonClicked);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(searchEdit); topLayout->addWidget(languageButton);
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addLayout(topLayout); leftLayout->addWidget(sqlTree);
    QWidget *leftContainer = new QWidget(this); leftContainer->setLayout(leftLayout);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(detailBrowser); rightLayout->addWidget(copyButton);
    QWidget *rightContainer = new QWidget(this); rightContainer->setLayout(rightLayout);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(leftContainer); splitter->addWidget(rightContainer);
    setCentralWidget(splitter);

    setWindowTitle("定仙游SQL");
    currentLang = "中文";
    loadSqlData();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::loadSqlData() {
    sqlTree->clear();
    bool isEn = (currentLang == "English");
    QString descKey = isEn ? "desc_en" : "desc_zh";
    QString codeKey = isEn ? "code_en" : "code_zh";

    QSqlQuery query("SELECT DISTINCT category FROM sql_statements ORDER BY category ASC");
    while (query.next()) {
        QString cat = query.value(0).toString();
        QTreeWidgetItem *catItem = new QTreeWidgetItem(sqlTree);
        catItem->setText(0, cat);
        QSqlQuery itemQuery;
        itemQuery.prepare("SELECT name, " + descKey + ", " + codeKey + " FROM sql_statements WHERE category = ?");
        itemQuery.addBindValue(cat);
        itemQuery.exec();
        while (itemQuery.next()) {
            QTreeWidgetItem *item = new QTreeWidgetItem(catItem);
            item->setText(0, itemQuery.value(0).toString());
            QString code = itemQuery.value(2).toString();
            // 已添加 white-space: pre-wrap; word-wrap: break-word; 修复显示溢出
            QString html = QString("<div style='font-family:Microsoft YaHei; margin:10px;'>"
                                   "<h2 style='color:#2c3e50;'>%1</h2>"
                                   "<p style='color:#555; font-size:14px; background:#f9f9f9; padding:8px; border-left:4px solid #3498db;'><b>%4 </b>%2</p>"
                                   "<b style='color:#2c3e50;'>%5</b>"
                                   "<pre style='background:#2d3748; color:#fff; padding:12px; border-radius:5px; font-family:Consolas,monaco,monospace; white-space: pre-wrap; word-wrap: break-word;'>%3</pre>"
                                   "</div>")
                               .arg(itemQuery.value(0).toString()).arg(itemQuery.value(1).toString())
                               .arg(code.toHtmlEscaped()).arg(isEn ? "Scenario:" : "业务情景:").arg(isEn ? "Code Template:" : "核心语法模版:");
            item->setData(0, Qt::UserRole, html);
            item->setData(0, Qt::UserRole + 1, code);
        }
    }
    sqlTree->expandAll();
}

void MainWindow::updateLanguage() {
    searchEdit->setPlaceholderText(currentLang == "English" ? "Search SQL statements..." : "搜索 SQL 语句...");
    copyButton->setText(currentLang == "English" ? "Copy Code" : "一键复制");
    loadSqlData();
    detailBrowser->clear();
}

void MainWindow::onTreeItemClicked(QTreeWidgetItem *item, int column) {
    detailBrowser->setHtml(item->data(0, Qt::UserRole).toString());
}

void MainWindow::onSearchTextChanged(const QString &text) {
    for (int i = 0; i < sqlTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *p = sqlTree->topLevelItem(i);
        bool showP = false;
        for (int j = 0; j < p->childCount(); ++j) {
            bool m = p->child(j)->text(0).contains(text, Qt::CaseInsensitive);
            p->child(j)->setHidden(!m);
            if (m) showP = true;
        }
        p->setHidden(!showP && !text.isEmpty());
        if (showP || text.isEmpty()) p->setExpanded(true);
    }
}

void MainWindow::onCopyButtonClicked() {
    QList<QTreeWidgetItem *> items = sqlTree->selectedItems();
    if (!items.isEmpty()) QApplication::clipboard()->setText(items.first()->data(0, Qt::UserRole + 1).toString());
}