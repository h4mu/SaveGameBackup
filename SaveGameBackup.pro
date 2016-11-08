TEMPLATE = subdirs

SUBDIRS += App

!android {
    SUBDIRS += Tests
}
