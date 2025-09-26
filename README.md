WeatherApp
Application météorologique Qt C++ qui affiche les données météo actuelles et prévisions 5 jours via l'API OpenWeatherMap.
Que fait le projet
L'application permet de rechercher une ville et d'afficher :

Conditions météorologiques actuelles (température, humidité, vent, pression)
Prévisions météo sur 5 jours avec résumés quotidiens
Graphiques d'évolution température/humidité
Localisation GPS de la ville

Le tout avec un système de cache intelligent pour optimiser les appels API.
Fonctionnalités livrées
1 Interface utilisateur complète

Layout en 2 panels : Météo (gauche) + Outils (droite)
Tous les cadres UI en place : Structure finale de l'interface
Design responsive avec groupes et sections organisées

2 Recherche météo fonctionnelle

Barre de recherche terminée avec validation
Auto-complétion basée sur l'historique
Gestion des erreurs avec messages utilisateur
États de chargement avec barre de progression

3 Données météo complètes

Météo actuelle : Température, ressenti, humidité, pression, vent, conditions
Prévisions 5 jours : Résumés quotidiens avec min/max et conditions
Coordonnées GPS : Latitude/longitude de la ville
Horodatage : Dernière mise à jour des données

4 Graphiques interactifs

Graphique température/humidité fonctionnel (QtCharts)
Double axe Y : °C (gauche) et % humidité (droite)
Courbes lissées sur 5 jours (40 créneaux de 3h)
Légende et couleurs différenciées

5 Système de cache intelligent

Cache automatique : 15min (météo) / 2h (prévisions)
Optimisation API : Évite les appels redondants
Nettoyage auto : Suppression des données expirées
Indicateurs cache dans les logs

6 Configuration robuste

Fichier config.json externe pour clé API
Validation : Format clé OpenWeatherMap (32 caractères hex)
Gestion d'erreurs : Messages explicites si config manquante
Sécurité : Clé API hors du code source

7 Architecture technique solide

WeatherService : Logique métier découplée
Signals/Slots Qt : Communication asynchrone
Structures typées : CurrentWeatherData, ForecastData
Parsing JSON robuste avec validation
Gestion d'erreurs multicouche

Fonctionnalités en développement
 Placeholders actifs

Historique des recherches : Interface créée, fonctionnalité en cours

Onglets Récent/Favoris préparés
Système de sauvegarde à connecter


Carte interactive : Zone réservée pour intégration future

Emplacement défini dans le panel droit
Coordonnées GPS disponibles pour affichage



Structure technique
Composants principaux

MainWindow : Interface utilisateur (panels gauche/droite)
WeatherService : Communication API et cache
WeatherChartWidget : Graphiques QtCharts
SimpleMapWidget : Affichage coordonnées
ConfigLoader : Gestion configuration

API utilisée

OpenWeatherMap API : Service météo gratuit
Endpoints : /weather (actuel) + /forecast (5 jours)
Format : JSON avec parsing vers structures C++
Limites : 1000 appels/jour (plan gratuit)

Installation
Prérequis

Qt 6.x (Core, Widgets, Network, Charts)
Clé API OpenWeatherMap gratuite

Configuration
Créer config.json dans le dossier de build :
json{
  "api_key": "votre_cle_32_caracteres"
}
Lancement

./WeatherApp
État du projet
Version actuelle : Interface complète avec fonctionnalités météo de base
Prochaines étapes : Finalisation historique et intégration carte
Architecture : Prête pour extensions futures
L'application fournit déjà une expérience météo complète avec les données essentielles, graphiques et optimisation cache. Les éléments d'interface sont en place pour les fonctionnalités avancées à venir.
