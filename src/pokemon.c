#include "pokemon.h"
#include "ataque.h"
#include "tipo.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pokemon {
	char nombre[20];
	enum TIPO tipo;
	struct ataque ataques[4];
};

struct info_pokemon {
	pokemon_t *pokemones;
	size_t cantidad;
};

void compare(pokemon_t *a, pokemon_t *b)
{
	pokemon_t temp = *a;
	*a = *b;
	*b = temp;
}

void ordenar_pokemones(informacion_pokemon_t *ip)
{
	for (size_t i = 0; i < ip->cantidad - 1; i++) {
		size_t min = i;
		for (size_t j = i + 1; j < ip->cantidad; j++) {
			if (strcmp(ip->pokemones[j].nombre,
				   ip->pokemones[min].nombre) < 0) {
				min = j;
			}
		}
		if (min != i) {
			compare(&ip->pokemones[i], &ip->pokemones[min]);
		}
	}
}

informacion_pokemon_t *pokemon_cargar_archivo(const char *path)
{
	if (path == NULL) {
		printf("Error al pasar el path del archivo.\n");
		return NULL;
	}
	informacion_pokemon_t *info = malloc(sizeof(informacion_pokemon_t));
	if (!info) {
		printf("Error al asignar memoria.\n");
		return NULL;
	}

	info->cantidad = 0;
	info->pokemones = NULL;

	FILE *archivo = fopen(path, "r");

	if (!archivo) {
		printf("No se pudo abrir el archivo (%s )\n", strerror(errno));
		free(info);
		return NULL;
	}

	pokemon_t *pokemon = malloc(sizeof(pokemon_t));
	while (!feof(archivo)) {
		char pokemon_type[20];
		int leidos = fscanf(archivo, "%[^;];%[^\n]\n", pokemon->nombre,
				    pokemon_type);
		if (leidos != 2) {
			fclose(archivo);
			free(pokemon);
			free(info);
			continue;
		}
		if (strcmp(pokemon_type, "F") == 0) {
			pokemon->tipo = FUEGO;
		} else if (strcmp(pokemon_type, "A") == 0) {
			pokemon->tipo = AGUA;
		} else if (strcmp(pokemon_type, "N") == 0) {
			pokemon->tipo = NORMAL;
		} else if (strcmp(pokemon_type, "P") == 0) {
			pokemon->tipo = PLANTA;
		} else if (strcmp(pokemon_type, "E") == 0) {
			pokemon->tipo = ELECTRICO;
		} else if (strcmp(pokemon_type, "R") == 0) {
			pokemon->tipo = ROCA;
		} else {
			printf("Tipo de Pokémon inválido.\n");
			fclose(archivo);
			free(pokemon);
			free(info);
			return NULL;
		}
		for (int i = 0; i < 3; i++) {
			char ataque_type[20];
			int ataques_leidos = fscanf(archivo, "%[^;];%[^;];%u\n",
						    pokemon->ataques[i].nombre,
						    ataque_type,
						    &pokemon->ataques[i].poder);
			if (ataques_leidos != 3) {
				printf("Error al leer los ataques del Pokémon.\n");
				fclose(archivo);
				free(pokemon);
				free(info);
				return NULL;
			}
			if (strcmp(ataque_type, "F") == 0) {
				pokemon->ataques[i].tipo = FUEGO;
			} else if (strcmp(ataque_type, "A") == 0) {
				pokemon->ataques[i].tipo = AGUA;
			} else if (strcmp(ataque_type, "N") == 0) {
				pokemon->ataques[i].tipo = NORMAL;
			} else if (strcmp(ataque_type, "P") == 0) {
				pokemon->ataques[i].tipo = PLANTA;
			} else if (strcmp(ataque_type, "E") == 0) {
				pokemon->ataques[i].tipo = ELECTRICO;
			} else if (strcmp(ataque_type, "R") == 0) {
				pokemon->ataques[i].tipo = ROCA;
			} else {
				printf("Tipo de Ataque inválido.\n");
				free(pokemon);
				fclose(archivo);
				return info;
			}
		}
		info->cantidad++;
		info->pokemones = realloc(info->pokemones,
					  sizeof(pokemon_t) * info->cantidad);
		if (!info->pokemones) {
			printf("Error al reasignar memoria para los Pokémon.\n");
			free(info);
			free(pokemon);
			fclose(archivo);
			return NULL;
		}
		info->pokemones[info->cantidad - 1] = *pokemon;
	}

	free(pokemon);
	ordenar_pokemones(info);
	fclose(archivo);

	return info;
}

pokemon_t *pokemon_buscar(informacion_pokemon_t *ip, const char *nombre)
{
	if (!ip || !nombre)
		return NULL;

	for (int i = 0; i < ip->cantidad; i++) {
		if (strcmp(ip->pokemones[i].nombre, nombre) == 0) {
			return &ip->pokemones[i];
		}
	}

	return NULL;
}

int pokemon_cantidad(informacion_pokemon_t *ip)
{
	if (ip) {
		return (int)ip->cantidad;
	} else
		return 0;
}

const char *pokemon_nombre(pokemon_t *pokemon)
{
	if (pokemon)
		return pokemon->nombre;

	return NULL;
}

enum TIPO pokemon_tipo(pokemon_t *pokemon)
{
	if (pokemon)
		return pokemon->tipo;
	else
		return NORMAL;
}

const struct ataque *pokemon_buscar_ataque(pokemon_t *pokemon,
					   const char *nombre)
{
	for (int i = 0; i < 3; i++) {
		if (strcmp(pokemon->ataques[i].nombre, nombre) == 0) {
			printf("Ataque encontrado: %s\n",
			       pokemon->ataques[i].nombre);
			return &pokemon->ataques[i];
		}
	}

	return NULL;
}

int con_cada_pokemon(informacion_pokemon_t *ip, void (*f)(pokemon_t *, void *),
		     void *aux)
{
	int cantidad_itinerados = 0;

	if (!ip || !f)
		return cantidad_itinerados;

	for (int i = 0; i < ip->cantidad; i++) {
		f(&ip->pokemones[i], aux);
		cantidad_itinerados++;
	}

	return cantidad_itinerados;
}

int con_cada_ataque(pokemon_t *pokemon,
		    void (*f)(const struct ataque *, void *), void *aux)
{
	int cantidad_itinerados = 0;

	if (!pokemon || !f)
		return cantidad_itinerados;

	for (int i = 0; i < 3; i++) {
		f(&pokemon->ataques[i], aux);
		cantidad_itinerados++;
	}

	return cantidad_itinerados;
}

void pokemon_destruir_todo(informacion_pokemon_t *ip)
{
	if (!ip)
		return;

	free(ip->pokemones);
	free(ip);
}
