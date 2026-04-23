#include "Exhibit.h"
#include "Guide.h"
#include "Museum.h"
#include "MuseumItem.h"
#include "Visitor.h"
#include <algorithm>
#include <clocale>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>

namespace {

void printSection(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

void showItemThroughBaseInterface(const MuseumItem& item) {
    item.printInfo();
    std::cout << "Рекомендуемое время осмотра: "
              << item.getRecommendedVisitMinutes() << " мин.\n";
}

void showRuntimeType(const MuseumItem& item) {
    std::cout << "Результат typeid: ";

    if (typeid(item) == typeid(Painting)) {
        std::cout << "объект является картиной.\n";
    } else if (typeid(item) == typeid(Sculpture)) {
        std::cout << "объект является скульптурой.\n";
    } else if (typeid(item) == typeid(Artifact)) {
        std::cout << "объект является артефактом.\n";
    } else {
        std::cout << "тип объекта определить не удалось.\n";
    }
}

void showSpecialDetails(const MuseumItem* item) {
    if (const Painting* painting = dynamic_cast<const Painting*>(item)) {
        std::cout << "dynamic_cast дал доступ к специфичному полю картины: техника = "
                  << painting->getTechnique() << ".\n";
        return;
    }

    if (const Sculpture* sculpture = dynamic_cast<const Sculpture*>(item)) {
        std::cout << "dynamic_cast дал доступ к специфичному полю скульптуры: материал = "
                  << sculpture->getMaterial() << ".\n";
        return;
    }

    if (const Artifact* artifact = dynamic_cast<const Artifact*>(item)) {
        std::cout << "dynamic_cast дал доступ к специфичному полю артефакта: эпоха = "
                  << artifact->getEpoch() << ".\n";
        return;
    }

    std::cout << "dynamic_cast не выявил специальный тип объекта.\n";
}

void showRouteItems(const MyContainer<std::shared_ptr<MuseumItem>>& route) {
    if (route.empty()) {
        std::cout << "Подходящих экспонатов для маршрута не найдено.\n";
        return;
    }

    for (const auto& item : route) {
        item->printInfo();
    }
}

void showItemsWithManualIterator(const MyContainer<std::shared_ptr<MuseumItem>>& items) {
    if (items.empty()) {
        std::cout << "Коллекция пуста, итератор показать не на чем.\n";
        return;
    }

    for (auto it = items.begin(); it != items.end(); ++it) {
        std::cout << "Элемент через ручной итератор:\n";
        (*it)->printInfo();
    }
}

}  // namespace

int main() {
    setlocale(LC_ALL, "");

    Museum museum("Городской историко-художественный музей");

    museum.addHall(1, "Русская живопись");
    museum.addHall(2, "Скульптура");
    museum.addHall(3, "Исторические реликвии");

    std::shared_ptr<MuseumItem> painting = std::make_shared<Painting>(
        "Девочка с персиками", 1887, "Валентин Серов", 1, "масло"
    );
    std::shared_ptr<MuseumItem> sculpture = std::make_shared<Sculpture>(
        "Мыслитель", 1904, "Огюст Роден", 2, "бронза"
    );
    std::shared_ptr<MuseumItem> artifact = std::make_shared<Artifact>(
        "Шлем княжеского дружинника", 1200, "Древняя Русь", 3, "XIII век"
    );
    std::shared_ptr<MuseumItem> secondPainting = std::make_shared<Painting>(
        "Над вечным покоем", 1894, "Исаак Левитан", 1, "масло"
    );

    museum.addItem(painting);
    museum.addItem(sculpture);
    museum.addItem(artifact);
    museum.addItem(secondPainting);

    Guide guide("Анна");
    guide.addToRoute(painting.get());
    guide.addToRoute(artifact.get());
    guide.addToRoute(sculpture.get());

    Visitor visitor("Иван");
    visitor.setGuide(&guide);

    printSection("Информация о музее");
    museum.showInfo();

    printSection("Залы музея");
    museum.showHalls();

    printSection("Коллекция экспонатов");
    museum.showCollection();

    Hall requestedHall(1, "Русская живопись");

    printSection("Полиморфный вывод");
    std::cout << "Ниже каждый объект обрабатывается через базовый интерфейс MuseumItem:\n";
    for (const auto& item : museum.getItems()) {
        showItemThroughBaseInterface(*item);
        std::cout << '\n';
    }

    printSection("Ручной обход коллекции итератором");
    std::cout << "Коллекция музея обходится через MyContainer::begin() / end()\n"
              << "и явный цикл for (auto it = begin(); it != end(); ++it).\n";
    showItemsWithManualIterator(museum.getItems());

    printSection("План осмотра");
    museum.showVisitPlan();

    printSection("Поиск зала через std::find");
    const Hall* foundHall = museum.findHall(requestedHall);
    if (foundHall != nullptr) {
        std::cout << "Зал найден в MyContainer<Hall>: ";
        foundHall->printInfo();
    } else {
        std::cout << "Запрошенный зал не найден.\n";
    }

    printSection("Прикладная задача семинара 5");
    std::cout << "Собираем короткий маршрут по залу \"" << requestedHall.getName()
              << "\" с ограничением до 15 минут на экспонат.\n"
              << "Поиск зала выполняется через std::find, а найденные экспонаты\n"
              << "сортируются в MyContainer по году через std::sort.\n";
    MyContainer<std::shared_ptr<MuseumItem>> shortRoute =
        museum.buildShortRouteForHall(requestedHall, 15);
    std::cout << "До сортировки:\n";
    showRouteItems(shortRoute);

    std::sort(shortRoute.begin(), shortRoute.end(), [](const auto& left, const auto& right) {
        if (left->getYear() != right->getYear()) {
            return left->getYear() < right->getYear();
        }

        return left->getTitle() < right->getTitle();
    });

    std::cout << "После сортировки по году и названию:\n";
    showRouteItems(shortRoute);

    if (!shortRoute.empty()) {
        std::cout << "Первая остановка маршрута через operator[]:\n";
        shortRoute[0]->printInfo();
    }

    MyContainer<std::shared_ptr<MuseumItem>> shortenedRoute = shortRoute;
    shortenedRoute.pop_back();
    std::cout << "После pop_back в сокращённом маршруте осталось остановок: "
              << shortenedRoute.size() << '\n';

    printSection("Маршрут экскурсии");
    std::cout << "Количество остановок в маршруте гида: " << guide.getRouteCount() << '\n';
    guide.showRoute();

    printSection("Работа гида и посетителя");
    visitor.askGuideName();

    printSection("Поиск через каталог");
    std::cout << "Музей делегирует поиск экспоната объекту Catalog.\n";
    const MuseumItem* foundItem = museum.findItem("Мыслитель");
    if (foundItem != nullptr) {
        std::cout << "Поиск завершён успешно. Найден объект:\n";
        foundItem->printInfo();
    } else {
        std::cout << "Экспонат не найден.\n";
    }

    printSection("RTTI и dynamic_cast");
    if (foundItem != nullptr) {
        std::cout << "Дополнительная проверка типа нужна только для доступа\n"
                  << "к данным, которых нет в базовом интерфейсе.\n";
        showRuntimeType(*foundItem);
        showSpecialDetails(foundItem);
    } else {
        std::cout << "RTTI не демонстрируется, потому что объект для проверки не найден.\n";
    }

    printSection("Итог");
    std::cout << "Программа показала композицию, агрегацию, ассоциацию,\n"
              << "делегирование и полиморфную работу с музейными объектами.\n";

    return 0;
}
