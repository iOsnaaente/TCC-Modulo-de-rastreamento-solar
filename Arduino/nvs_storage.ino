/*  
 *  Description: 
 *  Author: Bruno G. F. Sampaio
 *  Date: 04/01/2024
 *  License: MIT
 *
*/

#include "nvs_flash.h"
#include "nvs.h"

// /*
//  * Examplo de uso: 
//  * Dado a ser escrito  
//  *       >>> uint32_t data_to_write = 42;
//  * Chave para acessar o dado na NVS
//  *       >>> const char *key = "example_key";
//  * Escreve o dado na NVS
//  *       >>> write_to_nvs(key, &data_to_write, sizeof(data_to_write));
//  */
// bool write_to_nvs(const char *key, const void *data, size_t size) {
//     nvs_handle_t nvs_handle;
//     esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
//     if (err != ESP_OK) {
//         #ifdef DEBUG 
//             printf("Erro ao abrir o Namespace NVS\n");
//         #endif 
//         return false;
//     }
//     err = nvs_set_blob(nvs_handle, key, data, size);
//     if (err != ESP_OK) {
//         #ifdef DEBUG 
//             printf("Erro ao escrever dados no NVS\n");
//         #endif 
//         nvs_close(nvs_handle);
//         return false;
//     }
//     err = nvs_commit(nvs_handle);
//     if (err != ESP_OK) {
//         #ifdef DEBUG 
//             printf("Erro ao aplicar alterações no NVS\n");
//         #endif 
//         nvs_close(nvs_handle);
//         return false;
//     }
//     nvs_close(nvs_handle);
//     return true;
// }


// /*
//  * Examplo de uso: 
//  * Dado a ser lido
//  *      >>> uint32_t data_to_read = 0;
//  * Chave para acessar o dado na NVS
//  *      >>> const char *key = "example_key";
//  * Lê o dado da NVS
//  *      >>> read_from_nvs(key, &data_to_read, sizeof(data_to_read));
//  */
// bool read_from_nvs(const char *key, void *data, size_t size) {
//     nvs_handle_t nvs_handle;
//     esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
//     if (err != ESP_OK) {
//         #ifdef DEBUG 
//             printf("Erro ao abrir o Namespace NVS\n");
//         #endif
//         return false;
//     }
//     err = nvs_get_blob(nvs_handle, key, data, &size);
//     if (err != ESP_OK) {
//         #ifdef DEBUG 
//            printf("Erro ao ler dados do NVS\n");
//         #endif
//         nvs_close(nvs_handle);
//         return false;
//     }
//     nvs_close(nvs_handle);
//     return true;
// }