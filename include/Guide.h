#ifndef GUIDE_H
#define GUIDE_H

#include <string>
#include <vector>

class MuseumItem;

class Guide {
private:
    std::string name;
    std::vector<MuseumItem*> route;

public:
    Guide(const std::string& name);

    const std::string& getName() const;
    void addToRoute(MuseumItem* item);
    void showRoute() const;
};

#endif
