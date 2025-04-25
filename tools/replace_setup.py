Import("env")
import os
import shutil

print("Script extra_script exécuté !")

# Nom du dossier PlatformIO actuel (ex: esp32dev)
env_name = env["PIOENV"]

# Chemin vers la lib TFT_eSPI dans le dossier .pio
lib_path = os.path.join(env["PROJECT_DIR"], ".pio", "libdeps", env_name, "TFT_eSPI")

# Source (fichier à copier)
source_file = os.path.join(env["PROJECT_DIR"], "custom_setup", "User_Setup_Select.h")

# Destination (là où le fichier doit être copié)
target_file = os.path.join(lib_path, "User_Setup_Select.h")

# Si la lib est déjà là, on copie
if os.path.exists(lib_path):
    print(f"Copie de {source_file} vers {target_file}")
    shutil.copyfile(source_file, target_file)
else:
    print("La librairie TFT_eSPI n'a pas encore été installée.")
