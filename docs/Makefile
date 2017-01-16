SRC = $(wildcard *.md)
DST = $(patsubst %.md, html/%.html, $(SRC))

MD2HTML = pandoc
MKDIR	= mkdir -p
RM	= rm -rf

all: $(DST)

.PHONY: clean
clean:
	$(RM) $(DST)

html/%.html: %.md
	$(MKDIR) $(dir $@)
	$(MD2HTML) -o $@ -c default.css $<
