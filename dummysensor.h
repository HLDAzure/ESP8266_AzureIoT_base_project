void DummySensorRead() {
    AzureIotDevice->Reading = 10 + (rand() % 4 + 2);
}
