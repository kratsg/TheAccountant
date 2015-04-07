# TheAccountant - A RootCore Package

## Installing
The last stable analysis base used is **2.1.30**. To install,
```bash
mkdir myRootCore && cd $_
rcSetup Base,2.1.30
git clone https://github.com/kratsg/TheAccountant
git clone https://github.com/lawrenceleejr/Ext_RestFrames
rc find_packages
rc compile_pkg Ext_RestFrames
rc compile_pkg TheAccountant
```

## Dependencies
 - dependencies are in [cmt/Makefile.RootCore](cmt/Makefile.RootCore)
 - uses Chris Rogan's bad-ass [RestFrames](https://github.com/crogan/RestFrames) package

### Functionality Included

#### ClassifyEvent

This algorithm sets up a decay tree with a pair of vis-invis particles in each hemisphere and then decorates the eventInfo based on what it found.

### Tested Against AnalysisBase versions:
 - 2.1.30

#### Authors
- [Giordon Stark](https://github.com/kratsg)

##### Acknowledgements
- [Lawrence Lee](https://github.com/lawrenceleejr)
- [Chris Rogan](https://github.com/crogan)
