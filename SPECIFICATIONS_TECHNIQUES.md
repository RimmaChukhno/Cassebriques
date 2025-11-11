# Spécifications Techniques Détaillées

## 🎯 Architecture du Moteur de Jeu (Étape 1)

### GameObject - Classe de base

**Responsabilités :**
- Représenter tout objet visible et interactif dans le jeu
- Gérer position, taille, vitesse, rotation
- Fournir une zone de collision (AABB)
- Détecter les collisions avec d'autres objets

**Membres essentiels :**
```cpp
class GameObject {
protected:
    sf::Vector2f position;      // Position (x, y)
    sf::Vector2f size;          // Taille (width, height)
    sf::Vector2f velocity;      // Vitesse (vx, vy)
    float rotation;             // Angle de rotation en degrés
    AABB boundingBox;           // Zone de collision
    
public:
    virtual void update(float deltaTime);
    virtual void draw(sf::RenderWindow& window);
    bool checkCollision(const GameObject& other) const;
    AABB getAABB() const;
};
```

**AABB Structure :**
```cpp
struct AABB {
    float left, top, right, bottom;
    
    bool intersects(const AABB& other) const {
        return !(right < other.left || left > other.right ||
                 bottom < other.top || top > other.bottom);
    }
};
```

**Détection de collision :**
- Pour rectangle-rectangle : intersection AABB
- Pour cercle-cercle : distance entre centres < somme des rayons
- Pour cercle-rectangle : voir section bonus

### InputManager - Gestionnaire d'entrées

**Responsabilités :**
- Capturer les événements clavier et souris
- Permettre l'enregistrement de callbacks
- Gérer l'état des touches (pressed, released, held)

**Interface :**
```cpp
class InputManager {
private:
    std::map<sf::Keyboard::Key, std::vector<std::function<void()>>> keyCallbacks;
    std::map<sf::Mouse::Button, std::vector<std::function<void()>>> mouseCallbacks;
    sf::Vector2i mousePosition;
    
public:
    void processEvents(sf::Event& event);
    void registerKeyCallback(sf::Keyboard::Key key, std::function<void()> callback);
    void registerMouseCallback(sf::Mouse::Button button, std::function<void()> callback);
    sf::Vector2i getMousePosition() const;
    bool isKeyPressed(sf::Keyboard::Key key) const;
};
```

---

## 🎮 Casse-Briques Classique (Étape 2A)

### Paddle (Raquette)

**Hérite de :** GameObject

**Comportement :**
- Mouvement horizontal uniquement
- Vitesse constante lors du déplacement
- Collision avec la balle : calcul de l'angle de rebond selon le point d'impact

**Calcul de l'angle de rebond :**
```cpp
// Point d'impact sur la raquette (0.0 = gauche, 1.0 = droite)
float hitPos = (ballX - paddleX) / paddleWidth;
// Angle normalisé entre -60° et +60° (exemple)
float angle = (hitPos - 0.5f) * 120.0f;
```

### Ball (Balle)

**Hérite de :** GameObject (forme circulaire)

**Comportement :**
- Vitesse constante initiale
- Rebonds élastiques sur les murs
- Rebond sur la raquette avec angle calculé
- Destruction des briques au contact
- Perdue si y > screenHeight

**Rebond sur mur :**
```cpp
if (position.x - radius < 0 || position.x + radius > screenWidth) {
    velocity.x = -velocity.x;
}
if (position.y - radius < 0) {
    velocity.y = -velocity.y;
}
```

### Brick (Brique)

**Hérite de :** GameObject

**Propriétés :**
- Couleur selon le type
- Points de score
- Destruction immédiate au contact

### Game (Classe principale)

**Responsabilités :**
- Initialisation de la fenêtre SFML
- Boucle de jeu principale
- Gestion du score
- Conditions de victoire/défaite
- Augmentation progressive de la difficulté

**Boucle de jeu :**
```cpp
while (window.isOpen()) {
    float deltaTime = clock.restart().asSeconds();
    
    // Événements
    while (window.pollEvent(event)) {
        inputManager.processEvents(event);
    }
    
    // Mise à jour
    update(deltaTime);
    
    // Rendu
    window.clear();
    draw(window);
    window.display();
}
```

**Augmentation de la difficulté :**
- Multiplier la vitesse de la balle par un facteur (ex: 1.1) toutes les N secondes
- Ou augmenter la vitesse à chaque brique détruite

---

## 🚀 Casse-Briques Reborn (Étape 2B)

### Cannon (Canon)

**Hérite de :** GameObject

**Comportement :**
- Position fixe en bas de l'écran (centré horizontalement)
- Rotation pour pointer vers la souris
- Calcul de l'angle : `atan2(mouseY - cannonY, mouseX - cannonX)`

**Rotation :**
```cpp
sf::Vector2i mousePos = inputManager.getMousePosition();
float angle = atan2(mousePos.y - position.y, mousePos.x - position.x);
rotation = angle * 180.0f / M_PI;  // Conversion en degrés
```

### Projectile (Balle tirée)

**Hérite de :** GameObject (forme circulaire)

**Comportement :**
- Vitesse constante et rapide
- Direction = angle du canon au moment du tir
- Rebonds sur murs gauche, droit, haut
- Collision avec brique = rebond + dégâts
- Perdue si sortie par le bas

**Initialisation du tir :**
```cpp
float angleRad = cannon.getRotation() * M_PI / 180.0f;
velocity.x = speed * cos(angleRad);
velocity.y = speed * sin(angleRad);
```

### Brick (Brique avec HP)

**Hérite de :** GameObject

**Propriétés :**
- Points de vie (HP) : 1-3
- Couleur selon HP restant
- Affichage optionnel du nombre de HP

**Changement de couleur :**
```cpp
// Exemple de palette
if (hp == 3) color = sf::Color::Red;
else if (hp == 2) color = sf::Color::Yellow;
else if (hp == 1) color = sf::Color::Green;
```

**Prise de dégâts :**
```cpp
void takeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
        destroy();
    } else {
        updateColor();
    }
}
```

### GameReborn (Classe principale)

**Responsabilités :**
- Initialisation avec canon et grille de briques
- Gestion du tir (une seule balle active)
- Suivi de la souris pour le canon
- Gestion des collisions projectile-briques

**Limitation à une balle :**
```cpp
if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !hasActiveProjectile) {
    fireProjectile();
    hasActiveProjectile = true;
}

if (projectile.isOutOfBounds()) {
    hasActiveProjectile = false;
}
```

---

## 🎁 Fonctionnalités Bonus

### 1. Collision Cercle-AABB précise

**Algorithme :**
1. Trouver le point le plus proche du cercle sur le rectangle
2. Calculer la distance entre ce point et le centre du cercle
3. Si distance < rayon, collision

```cpp
bool circleAABBCollision(const sf::Vector2f& circleCenter, float radius, const AABB& box) {
    float closestX = std::max(box.left, std::min(circleCenter.x, box.right));
    float closestY = std::max(box.top, std::min(circleCenter.y, box.bottom));
    
    float distanceX = circleCenter.x - closestX;
    float distanceY = circleCenter.y - closestY;
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;
    
    return distanceSquared < (radius * radius);
}
```

### 2. AssetManager (Singleton)

**Pattern Singleton :**
```cpp
class AssetManager {
private:
    static AssetManager* instance;
    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::Font> fonts;
    
    AssetManager() {}  // Constructeur privé
    
public:
    static AssetManager* getInstance();
    sf::Texture& getTexture(const std::string& name);
    sf::Font& getFont(const std::string& name);
    void loadTexture(const std::string& name, const std::string& path);
    void loadFont(const std::string& name, const std::string& path);
};
```

### 3. Chargement de niveaux

**Format de fichier :**
```
5 10
3 2 1 2 3 2 1 2 3 2
2 1 2 1 2 1 2 1 2 1
1 1 1 1 1 1 1 1 1 1
2 2 2 2 2 2 2 2 2 2
3 3 3 3 3 3 3 3 3 3
```

**Parser :**
```cpp
std::vector<std::vector<int>> loadLevel(const std::string& filename) {
    std::ifstream file(filename);
    int rows, cols;
    file >> rows >> cols;
    
    std::vector<std::vector<int>> level(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            file >> level[i][j];
        }
    }
    return level;
}
```

### 4. Multi-tirs simultanés

**Gestion :**
```cpp
std::vector<Projectile> projectiles;
const int MAX_PROJECTILES = 3;

void fireProjectile() {
    if (projectiles.size() < MAX_PROJECTILES) {
        projectiles.emplace_back(cannon.getPosition(), cannon.getRotation());
    }
}

void updateProjectiles(float deltaTime) {
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& p) { return p.isOutOfBounds(); }),
        projectiles.end()
    );
    
    for (auto& projectile : projectiles) {
        projectile.update(deltaTime);
    }
}
```

### 5. Onde de choc (Clic droit)

**Comportement :**
- Zone rectangulaire dans la direction du canon
- Repousse toutes les balles touchées
- Animation visuelle

```cpp
class Shockwave {
    sf::RectangleShape shape;
    sf::Vector2f direction;
    float speed;
    float lifetime;
    
public:
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    bool affectsProjectile(const Projectile& p) const;
    void pushProjectile(Projectile& p);
};
```

---

## 📐 Dimensions et constantes recommandées

### Fenêtre
- Largeur : 800 ou 1024 pixels
- Hauteur : 600 ou 768 pixels

### Raquette (Classique)
- Largeur : 100-150 pixels
- Hauteur : 20 pixels
- Position Y : height - 50

### Canon (Reborn)
- Largeur : 40 pixels
- Hauteur : 60 pixels
- Position Y : height - 80

### Balle/Projectile
- Rayon : 8-10 pixels
- Vitesse initiale : 300-400 pixels/seconde

### Briques
- Largeur : 60-80 pixels
- Hauteur : 30-40 pixels
- Espacement : 5-10 pixels

### Grille de briques
- Lignes : 5-8
- Colonnes : 10-12
- Position Y de départ : 50-100 pixels du haut

---

## 🎨 Couleurs recommandées

### Casse-Briques Classique
- Fond : Noir ou bleu foncé
- Raquette : Blanc ou gris
- Balle : Blanc ou jaune
- Briques : Palette variée (rouge, orange, jaune, vert, bleu)

### Casse-Briques Reborn
- Fond : Noir ou gris foncé
- Canon : Gris ou blanc
- Projectile : Blanc ou cyan
- Briques selon HP :
  - 3 HP : Rouge
  - 2 HP : Orange/Jaune
  - 1 HP : Vert

---

## ⚡ Optimisations possibles

1. **Spatial Partitioning** : Diviser l'écran en zones pour limiter les tests de collision
2. **Object Pooling** : Réutiliser les objets (projectiles, briques) au lieu de créer/détruire
3. **Culling** : Ne pas dessiner les objets hors écran
4. **Delta Time** : Utiliser le temps écoulé pour des mouvements indépendants de la fréquence d'images

---

## 🧪 Tests recommandés

1. **Collisions :**
   - Balle-mur (tous les bords)
   - Balle-raquette (différents points d'impact)
   - Balle-brique (coins et bords)

2. **Gameplay :**
   - Score correct
   - Conditions de victoire/défaite
   - Augmentation de la difficulté

3. **Reborn :**
   - Rotation du canon
   - Limitation à une balle
   - Système de HP des briques

