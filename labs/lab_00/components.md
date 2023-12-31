# Компонентная архитектура
<!-- Состав и взаимосвязи компонентов системы между собой и внешними системами с указанием протоколов, ключевые технологии, используемые для реализации компонентов.
Диаграмма контейнеров C4 и текстовое описание. 
-->
## Компонентная диаграмма

```plantuml
@startuml
!include https://raw.githubusercontent.com/plantuml-stdlib/C4-PlantUML/master/C4_Container.puml

AddElementTag("microService", $shape=EightSidedShape(), $bgColor="CornflowerBlue", $fontColor="white", $legendText="microservice")
AddElementTag("storage", $shape=RoundedBoxShape(), $bgColor="lightSkyBlue", $fontColor="white")

Person(admin, "Администратор")
Person(user, "Пользователь")

System_Ext(web_site, "Клиентский веб-сайт", "HTML, CSS, JavaScript, React", "Веб-интерфейс")

System_Boundary(messenger, "Мессенджер") {
   Container(client_service, "Сервис авторизации", "C++", "Сервис управления пользователями", $tags = "microService")    
   Container(group_service, "Сервис групповых чатов", "C++", "Сервис управления групповыми чатами", $tags = "microService")
   Container(ptp_service, "Сервис PtP чатов", "C++", "Сервис управления PtP чатами", $tags = "microService")   
   ContainerDb(db, "База данных", "MySQL", "Хранение данных о сообщениях, чатах и пользователях", $tags = "storage")
   
}

Rel(admin, web_site, "Просмотр, добавление и редактирование информации о пользователях")
Rel(user, web_site, "Регистрация, отправка сообщений другим пользователем, создание групповых чатов")

Rel(web_site, client_service, "Работа с пользователями")
Rel(client_service, db, "INSERT/SELECT/UPDATE", "SQL")

Rel(web_site, group_service, "Работа с групповыми чатами")
Rel(group_service, db, "INSERT/SELECT/UPDATE", "SQL")

Rel(web_site, ptp_service, "Работа с ptp чатами")
Rel(ptp_service, db, "INSERT/SELECT/UPDATE", "SQL")

@enduml
```
## Список компонентов  

### Сервис авторизации
**API**:
-	Создание нового пользователя
  - Входные параметры: login, пароль, имя, фамилия, email
  - Выходные параметры: отсутствуют
-	Поиск пользователя по логину
  - Входные параметры: login
  - Выходные параметры: id пользователя
-	Поиск пользователя по маске имени и фамилии
  - Входные параметры: маска фамилии, маска имени
  - Выходные параметры: login, имя, фамилия, email

### Сервис груповых чатов
**API**:
- Создание группового чата
  - Входные параметры: название чата
  - Выходные параметры: id чата
- Добавления пользователя в чат
  - Входные параметры: id чата, login пользователя
  - Выходные параметры: отсутствуют
- Добавление сообщения в групповой чат
  - Входные параметры: id чата, login пользователя, текст сообщения
  - Выходные параметры: id сообщения
- Загрузка сообщений группового чата
  - Входные параметры: id чата
  - Выходные параметры: массив сообщений (id, id чата, id автора, содержание, дата добавления)

### Сервис PtP чатов
**API**:
- Создание PtP чата
  - Входные параметры: login участника 1, login участника 2
  - Выходные параметры: id чата
- Отправка PtP сообщения пользователю
  - Входные параметры: login отправителя, login получателя, сообщение
  - Выходные параметры: id сообщение
- Получение  списка сообщения PtP чата
  - Входные параметры: login участника 1, login участника 2
  - Выходные параметры: массив сообщений (id, id чата, id авторa, содержание, дата добавления)
